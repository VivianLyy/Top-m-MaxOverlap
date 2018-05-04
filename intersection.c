
#include "intersection.h"
//计算两个圆的交点
void inter_computeIntersectionTwoCirlce(circle *aCircle1, circle *aCircle2, double *x1, double *y1, double *x2, double *y2)
{
	double a, b, r;
	double c, d, s;

	double temp;
	double temp2;
	double temp3;
	double temp4;
	double temp5;
	double temp6;

	double delta;

	double A, B, C;

	a = aCircle1->x;
	b = aCircle1->y;
	r = aCircle1->radius;

	c = aCircle2->x;
	d = aCircle2->y;
	s = aCircle2->radius;

	if ((a == c) && (b != d))
	{
		temp = (a*a + b*b - r*r - (c*c + d*d - s*s))/(2*(b-d));
		delta = r*r - (temp-b)*(temp-b);
		if (delta >= 0)
		{
			temp2 = sqrt(delta);

			(*y1) = temp;
			(*x1) = a + temp2;


			(*y2) = temp;
			(*x2) = a - temp2;
		}
		else
		{
			// invalid
			(*y1) = 0;
			(*x1) = 0;


			(*y2) = 0;
			(*x2) = 0;
		}
	}
	else if ((a != c) && (b == d))
	{
		temp = (a*a + b*b - r*r - (c*c + d*d - s*s))/(2*(a-c));
		delta = r*r - (temp-a)*(temp-a);
		if (delta >= 0)
		{
			temp2 = sqrt(delta);

			(*x1) = temp;
			(*y1) = b + temp2;


			(*x2) = temp;
			(*y2) = b - temp2;
		}
		else
		{
			// invalid
			(*y1) = 0;
			(*x1) = 0;


			(*y2) = 0;
			(*x2) = 0;
		}
	}
	else if ((a != c) && (b != d))
	{
		temp = (a-c)/(b-d);
		temp2 = a*a + b*b - r*r - (c*c + d*d - s*s);
		temp3 = temp2/(2*(b-d)) - b;

		A = 1 + temp*temp;
		B = 2*b*(a-c)/(b-d) - 2*a - temp2*(a-c)/((b-d)*(b-d));
		C = temp3*temp3 + a*a - r*r;

		delta = B*B - 4*A*C;
		if (delta >= 0)
		{
			temp4 = sqrt(delta);

			(*x1) = (-1*B + temp4)/(2*A);
			(*x2) = (-1*B - temp4)/(2*A);

			temp5 = (a*a + b*b - r*r - (c*c + d*d - s*s))/(2*(b-d));

			temp6 = (a-c)/(b-d);

			(*y1) = temp5 - temp6*(*x1);
			(*y2) = temp5 - temp6*(*x2);
		}
		else
		{
			// invalid
			(*y1) = 0;
			(*x1) = 0;


			(*y2) = 0;
			(*x2) = 0;
		}
	}
	else
	{
		// a = c and b = d 
		// not possible
		(*x1) = 0;
		(*x2) = 0;

		(*y1) = 0;
		(*y2) = 0;
	}
}


