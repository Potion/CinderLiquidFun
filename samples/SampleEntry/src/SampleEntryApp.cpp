/*
* Copyright (c) 2006-2007 Erin Catto http://www.box2d.org
* Copyright (c) 2013 Google, Inc.
* Copyright (c) 2015 The Cinder Project
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "Box2D/Box2D.h"
using namespace ci;
using namespace ci::app;
using namespace std;

#include "Render.h"
#include "Test.h"
#include "Arrow.h"
#include "FullscreenUI.h"
#include "ParticleParameter.h"


// --------------particle scene
#include "SampleEntry.h"
namespace TestMain
{
	//-----------------------------------------------declare functions-------------------------------
	// Set whether to restart the test on particle parameter changes.
	// This parameter is re-enabled when the test changes.
	void SetRestartOnParticleParameterChange( bool enable );

	// Set the currently selected particle parameter value.  This value must
	// match one of the values in TestMain::k_particleTypes or one of the values
	// referenced by particleParameterDef passed to SetParticleParameters().
	uint32 SetParticleParameterValue( uint32 value );

	// Get the currently selected particle parameter value and enable particle
	// parameter selection arrows on Android.
	uint32 GetParticleParameterValue();

	// Override the default particle parameters for the test.
	void SetParticleParameters(
	    const ParticleParameter::Definition* const particleParameterDef,
	    const uint32 particleParameterDefCount );

	//-----------------------------------------------define functions-------------------------------
	namespace
	{
		Test* test;
		Settings settings;
		int32 width = 640;
		int32 height = 540;
		float settingsHz = 60.0;

		float32 viewZoom = 1.0f;
		int tx, ty, tw, th;
		bool rMouseDown = false;

		// State of the mouse on the previous call of Mouse().
		int32 previousMouseState = -1;
		b2Vec2 lastp;
		b2Vec2 extents;

		// Fullscreen UI object.
		FullscreenUI fullscreenUI;
		// Used to control the behavior of particle tests.
		ParticleParameter particleParameter;
	}

	void SetRestartOnParticleParameterChange( bool enable )
	{
		particleParameter.SetRestartOnChange( enable );
	}

	uint32 SetParticleParameterValue( uint32 value )
	{
		const int32 index = particleParameter.FindIndexByValue( value );
		// If the particle type isn't found, so fallback to the first entry in the
		// parameter.
		particleParameter.Set( index >= 0 ? index : 0 );
		return particleParameter.GetValue();
	}

	uint32 GetParticleParameterValue()
	{
		// Enable display of particle type selection arrows.
		fullscreenUI.SetParticleParameterSelectionEnabled( true );
		return particleParameter.GetValue();
	}

	void SetParticleParameters(
	    const ParticleParameter::Definition* const particleParameterDef,
	    const uint32 particleParameterDefCount )
	{
		particleParameter.SetDefinition( particleParameterDef,
		                                 particleParameterDefCount );
	}

}


using namespace TestMain;

static b2Vec2 ConvertScreenToWorld( int32 x, int32 y )
{
	float32 u = x / float32( tw );
	float32 v = ( th - y ) / float32( th );

	b2Vec2 lower = settings.viewCenter - extents;
	b2Vec2 upper = settings.viewCenter + extents;

	b2Vec2 p;
	p.x = ( 1.0f - u ) * lower.x + u * upper.x;
	p.y = ( 1.0f - v ) * lower.y + v * upper.y;
	return p;
}

//---------------------------------------------------- mainApp.cpp ------------------------------------
class SampleEntryApp : public App
{
	public:
		void setup() override;
		void mouseUp( MouseEvent event ) override;
		void mouseDrag( MouseEvent event ) override;
		void mouseDown( MouseEvent event ) override;
		void update() override;
		void draw() override;
		virtual void resize() override;
		void resizeView( int width, int height );

};

void SampleEntryApp::setup()
{
	test = SampleEntry::Create();
}

void SampleEntryApp::mouseUp( MouseEvent event )
{
	// -------------connect to ui mouse events!

	int x = event.getPos().x;
	int y = event.getPos().y;

	int state = GLUT_NONE;

	if( event.isLeft() ) {
		state = GLUT_UP;
		b2Vec2 p = ConvertScreenToWorld( x, y );
		test->MouseUp( p );
	}
	else if( event.isRight() ) {
		state = GLUT_UP;
		rMouseDown = false;
	}

	previousMouseState = state;

}
void SampleEntryApp::mouseDrag( MouseEvent event )
{
	// -------------connect to ui mouse events!

	int x = event.getPos().x;
	int y = event.getPos().y;
	b2Vec2 p = ConvertScreenToWorld( x, y );

	if( fullscreenUI.GetSelection() == FullscreenUI::e_SelectionNone ) {
		test->MouseMove( p );
	}

	if( rMouseDown ) {
		b2Vec2 diff = p - lastp;
		settings.viewCenter.x -= diff.x;
		settings.viewCenter.y -= diff.y;
		resizeView( width, height );
		lastp = ConvertScreenToWorld( x, y );
	}

}
void SampleEntryApp::mouseDown( MouseEvent event )
{

	// -------------connect to ui mouse events!
	int x = event.getPos().x;
	int y = event.getPos().y;
	int button = GLUT_NONE;
	int state = GLUT_NONE;
	int mod = GLUT_NONE;

	if( event.isLeft() ) {
		button = GLUT_LEFT_BUTTON;

		if( event.isLeftDown() ) {
			state = GLUT_DOWN;
		}
	}
	else if( event.isRight() ) {
		button = GLUT_RIGHT_BUTTON;

		if( event.isRightDown() ) {
			state = GLUT_DOWN;
		}
	}

	if( button == GLUT_LEFT_BUTTON ) {
		b2Vec2 p = ConvertScreenToWorld( x, y );

		switch( fullscreenUI.Mouse( button, state, previousMouseState, p ) ) {

			case FullscreenUI::e_SelectionParameterPrevious:
				particleParameter.Decrement();
				break;

			case FullscreenUI::e_SelectionParameterNext:
				particleParameter.Increment();
				break;

			default:
				break;
		}

		if( state == GLUT_DOWN ) {
			b2Vec2 p = ConvertScreenToWorld( x, y );

			if( mod == GLUT_ACTIVE_SHIFT ) {
				test->ShiftMouseDown( p );
			}
			else {
				test->MouseDown( p );
			}
		}
	}
	else if( button == GLUT_RIGHT_BUTTON ) {
		if( state == GLUT_DOWN ) {
			lastp = ConvertScreenToWorld( x, y );
			rMouseDown = true;
		}
	}

	previousMouseState = state;

}

void SampleEntryApp::resizeView( int viewWidth, int viewHeight )
{
	width = viewWidth;
	height = viewHeight;

	float pixelScale = getWindowContentScale();

	tx = 0;
	ty = 0;
	tw = width;
	th = height;

	gl::viewport( tx * pixelScale, ty * pixelScale, tw * pixelScale, th * pixelScale );

	float32 ratio = th ? float32( tw ) / float32( th ) : 1;

	extents = ratio >= 1 ? b2Vec2( ratio * 25.0f, 25.0f ) : b2Vec2( 25.0f, 25.0f / ratio );
	extents *= viewZoom;

	b2Vec2 lower = settings.viewCenter - extents;
	b2Vec2 upper = settings.viewCenter + extents;

	// L/R/B/T
	LoadOrtho2DMatrix( getWindowWidth(), getWindowHeight(), lower.x, upper.x, lower.y, upper.y );

	if( fullscreenUI.GetEnabled() ) {
		fullscreenUI.SetViewParameters( &settings.viewCenter, &extents );
	}
}

void SampleEntryApp::resize()
{
	resizeView( getWindowWidth(), getWindowHeight() );
}
void SampleEntryApp::update()
{
	test->Update();
}

void SampleEntryApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
	settings.hz = settingsHz;
	test->Step( &settings );

	// zoom the view
	viewZoom = test->GetDefaultViewZoom();
	settings.viewCenter.Set( 0.0f, 20.0f * viewZoom );

	// keep zoom inline with window size
	resizeView( width, height );
	ci::gl::color( ci::Color( 1.f, 1.f, 1.f ) );
	ci::gl::drawString( to_string( getAverageFps() ), ci::vec2( 20.f ), ci::Color::white() );
	ci::app::console() << "fps" << getAverageFps() << std::endl;

}

CINDER_APP( SampleEntryApp, RendererGl )
