#include "Light.h"
#include "Logger.h"
#include "Sound.h"

Light::Light()
{ }

void Light::setRed (unsigned char newRed, unsigned char affectedMod)
{
	red[affectedMod == CURRENT_MOD ? mod : affectedMod] = newRed;
}

void Light::setGreen (unsigned char newGreen, unsigned char affectedMod)
{
	green[affectedMod == CURRENT_MOD ? mod : affectedMod] = newGreen;
}

void Light::setBlue (unsigned char newBlue, unsigned char affectedMod)
{
	blue[affectedMod == CURRENT_MOD ? mod : affectedMod] = newBlue;
}

void Light::setRgb (unsigned long newRgb, unsigned char affectedMod)
{
	red[affectedMod == CURRENT_MOD ? mod : affectedMod]   = (unsigned char) (newRgb >> 16);
	green[affectedMod == CURRENT_MOD ? mod : affectedMod] = (unsigned char) (newRgb >> 8);
	blue[affectedMod == CURRENT_MOD ? mod : affectedMod]  = (unsigned char) newRgb;
}

void Light::setPower (unsigned char newPower, unsigned char affectedMod)
{
	power[affectedMod == CURRENT_MOD ? mod : affectedMod] = newPower;
}

void Light::setSpeed (int newSpeed, unsigned char affectedMod)
{
	speed[affectedMod == CURRENT_MOD ? mod : affectedMod] = newSpeed;
}

void Light::setMod (unsigned char newMod)
{
	mod = newMod;
}

void Light::switchOn ()
{
	on = true;
}

void Light::switchOff ()
{
	on = false;
}

int Light::getDawnDuration ()
{
	return speed[LIGHT_MOD_DAWN];
}

unsigned char Light::getRed (unsigned char affectedMod)
{
	return red[affectedMod == CURRENT_MOD ? mod : affectedMod];
}

unsigned char Light::getGreen (unsigned char affectedMod)
{
	return green[affectedMod == CURRENT_MOD ? mod : affectedMod];
}

unsigned char Light::getBlue (unsigned char affectedMod)
{
	return blue[affectedMod == CURRENT_MOD ? mod : affectedMod];
}

unsigned long Light::getRgb (unsigned char affectedMod)
{
	return (((unsigned long) red[affectedMod == CURRENT_MOD ? mod : affectedMod]) << 16) + (((unsigned long) green[affectedMod == CURRENT_MOD ? mod : affectedMod]) << 8) + blue[affectedMod == CURRENT_MOD ? mod : affectedMod];
}

unsigned char Light::getPower (unsigned char affectedMod)
{
	return power[affectedMod == CURRENT_MOD ? mod : affectedMod];
}

unsigned int Light::getSpeed (unsigned char affectedMod)
{
	return speed[affectedMod == CURRENT_MOD ? mod : affectedMod];
}

unsigned char Light::getMod ()
{
	return mod;
}

bool Light::isOn ()
{
	return on != 0;
}

bool Light::isOff ()
{
	return on == 0;
}

Light light = Light();
