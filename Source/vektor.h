#ifndef VEKTOR_H_
#define VEKTOR_H_
class vektor
	{
	public:
		double x;
		double y;
		vektor(double xp=0.0 , double yp=0.0);
		~vektor();
		vektor operator+ (const vektor& other)const;
		vektor operator=(const vektor& other);
		vektor operator*(int lambda) const;
		//két vektor hajlásszögét adja vissza
		double operator>(const vektor& other)const;
		double abs()const;
		double distance(const vektor& other)const;
	};
vektor operator*(int lambda, const vektor &other);
#endif