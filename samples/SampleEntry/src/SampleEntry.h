#include "cinder\app\App.h"
#include "Test.h"

class SampleEntry : public Test
{
	public:
		SampleEntry()
		{
			{
				const b2Vec2 vertices[4] = {
					b2Vec2( -2, 0 ),
					b2Vec2( 2, 0 ),
					b2Vec2( 2, 4 ),
					b2Vec2( -2, 4 )
				};
				shape.CreateLoop( vertices, 4 );
				ground->CreateFixture( &shape, 0.0f );

			}

			// ----------- creating dynamic body
			{
				b2BodyDef  dynamicBody;
				dynamicBody.type = b2_dynamicBody;
				b2Body* body = m_world->CreateBody( &dynamicBody );
				b2CircleShape  circleShape;		// cirlce shape, box shape or any shape
				circleShape.m_p.Set( 1.f, 1.f ); // setting shape position
				circleShape.m_radius = 0.3f; // setting shape radius
				body->CreateFixture( &circleShape, 0.5f );
			}

			// ----------- creating particle system
			m_particleSystem->SetRadius( 0.1f );
			m_particleSystem->SetDamping( 0.99 );
			{
				// create particle in a particular shape
				b2CircleShape shape;		// particle group init container size, particles will fill in this container
				shape.m_p.Set( 1.f, 1.f ); // setting particle group location
				shape.m_radius = 1; // setting the particle group size

				b2ParticleGroupDef pd;
				// flag is setting particle type - behaviour
				pd.flags = b2_waterParticle;
				m_origColor.Set( 255, 255, 0, 255 );
				pd.color = m_origColor;
				pd.shape = &shape;

				b2ParticleGroup* const group = m_particleSystem->CreateParticleGroup( pd );

				if( pd.flags & b2_colorMixingParticle ) {
					ColorParticleGroup( group, 0 );
				}
			}

		}

		void Update()
		{
			// update container structure in reatime
			ground->DestroyFixture( ground->GetFixtureList() );

			const b2Vec2 vertices[4] = {
				b2Vec2( sin( ci::app::getElapsedSeconds() ), 0 ),
				b2Vec2( 2, sin( ci::app::getElapsedSeconds() ) * 0.3f ),
				b2Vec2( cos( ci::app::getElapsedSeconds() ) + 2.f, 4 ),
				b2Vec2( -2, sin( ci::app::getElapsedSeconds() ) + 4 )
			};
			shape.CreateLoop( vertices, 4 );

			ground->CreateFixture( &shape, 0.0f );
		}

		float32 GetDefaultViewZoom() const
		{
			return 0.1f;
		}

		static Test* Create()
		{
			return new SampleEntry;
		}

	private:
		b2BodyDef bd;
		b2Body* ground = m_world->CreateBody( &bd );
		b2ChainShape shape;
		b2ParticleColor m_origColor;

};
