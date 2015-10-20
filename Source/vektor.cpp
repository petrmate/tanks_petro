#include "vektor.h"
#include <math.h>

vektor::vektor(double xp , double yp ) :x(xp), y(yp)
	{}
vektor::~vektor()
	{}
vektor vektor::operator+ (const vektor& other)const
	{
	vektor a;
	a.x = x + other.x;
	a.y = y + other.y;
	return a;
	}
double vektor::abs()const
	{
	return sqrt(x*x + y*y);
	}
double vektor::operator>(const vektor& other)const
	{
	double cos = (x*other.x + y*other.y) / (abs()*other.abs()); //szögszámítás skaláris szorzattal
	return acos(cos);
	}
vektor vektor::operator=(const vektor& other)
	{
	x = other.x;
	y = other.y;
	return *this;
	}
vektor vektor::operator*(int lambda) const
	{
	vektor a;
	a.x = x*lambda;
	a.y = y*lambda;
	return a;
	}
vektor operator*(int lambda, const vektor &other)
	{
	vektor a;
	a.x = lambda*other.x;
	a.y = lambda*other.y;
	return a;
	}

double vektor::distance(const vektor& other)const
	{
	return sqrt((x - other.x)*(x - other.x) + (y - other.y)*(y - other.y));
	}