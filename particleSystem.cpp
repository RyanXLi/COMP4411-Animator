#pragma warning(disable : 4786)

#include "particleSystem.h"


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
// add
#include <ctime>
#include <FL/gl.h>
#include "modelerdraw.h"


/***************
 * Constructors
 ***************/

ParticleSystem::ParticleSystem() 
{
	// TODO
	bake_fps = 30;
	bake_start_time = -1;  // meaningless
	bake_end_time = -1;	   // meaningless
	simulate = false;
	dirty = false;
	srand(time(0));
	force_vec.push_back(new Gravity(Vec3d(0, -9.8, 0)));
}





/*************
 * Destructor
 *************/

ParticleSystem::~ParticleSystem() 
{
	// TODO
	for (auto it = force_vec.begin(); it != force_vec.end(); ++it) { delete (*it); }
}


/******************
 * Simulation fxns
 ******************/

/** Start the simulation */
void ParticleSystem::startSimulation(float t)
{
    
	// TODO
	clearBaked();
	bake_start_time = t;
	// These values are used by the UI ...
	// -ve bake_end_time indicates that simulation
	// is still progressing, and allows the
	// indicator window above the time slider
	// to correctly show the "baked" region
	// in grey.
	bake_end_time = -1;
	simulate = true;
	dirty = true;

}

/** Stop the simulation */
void ParticleSystem::stopSimulation(float t)
{
    
	// TODO
	// bake_start_time = -1;
	bake_end_time = t;
	// These values are used by the UI
	simulate = false;
	dirty = true;

}

/** Reset the simulation */
void ParticleSystem::resetSimulation(float t)
{
    
	// TODO
	clearBaked();
	bake_start_time = -1;
	bake_end_time = -1;
	// These values are used by the UI
	simulate = false;
	dirty = true;

}

/** Compute forces and update particles **/
void ParticleSystem::computeForcesAndUpdateParticles(float t)
{
	// TODO
	float dt = 1 / bake_fps;
	if (isSimulate()) {
		if (!isBakedAt(t)) {
			printf("updateParticles!\n");
			for (auto it = particle_vec.begin(); it != particle_vec.end(); ++it) {
				//std::cout << it->getP() << it->getM() << std::endl;
				//std::cout << it->getV() << it->getF() << std::endl;
				it->update(dt);
			}
			bakeParticles(t);
		}
	}
}


/** Render(draw) particles */
void ParticleSystem::drawParticles(float t)
{
	if (isBakedAt(t)) {
		std::vector<Particle> bakedparticle = bake_storage.at(t);
		for (auto it = bakedparticle.begin(); it != bakedparticle.end(); ++it) {
			printf("drawParticles!\n");
			it->draw();
		}
	}
	// TODO
}





/** Adds the current configuration of particles to
  * your data structure for storing baked particles **/
void ParticleSystem::bakeParticles(float t) 
{
	bake_storage.insert(std::pair<float, std::vector<Particle> >(t, particle_vec));
	// TODO
}

/** Clears out your data structure of baked particles */
void ParticleSystem::clearBaked()
{
	bake_storage.clear();
	// TODO
}

bool ParticleSystem::isBakedAt(float t)
{
	auto it = bake_storage.find(t);
	return (it != bake_storage.end());
}

void ParticleSystem::spawnParticles(Mat4f CameraM, Mat4f CurrModelM, float currt)
{
	Mat4f WorldM = CameraM.inverse() * CurrModelM;
	Vec4f WorldP4 = WorldM * Vec4f(0, 0, 0, 1);

	if (isSimulate()) {
		printf("spawnParticles!\n");
		Vec3d WorldP(WorldP4[0], WorldP4[1]+ rand() % 80 / 100.0 - 0.4, WorldP4[2] + rand() % 80 / 100.0 - 0.4);
		if (!isBakedAt(currt + 1.0f / bake_fps)) {
			double mass = (rand() % 50) / 5.0 + 0.5;
			Particle newP(WorldP, mass);
			Vec3d velocity(rand()%3+2.0, rand()%100/100.0*2 - 1, rand() % 100*2 / 100.0);
			newP.setV(velocity);
			for (std::vector<Force*>::iterator it = force_vec.begin(); it != force_vec.end(); ++it) {
				newP.addForce(*it);
			}
			particle_vec.push_back(newP);
		}
	}
}





