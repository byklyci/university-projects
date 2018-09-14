%-----------------------------------
% Problem 3
%-----------------------------------

figure;
x = -100:100;

subplot(4,2,1);
y1 = sin(x);
plot(x,y1);
title('Problem 3: sin(x)')

subplot(4,2,2);
y2 = sin(50.*x);
plot(x,y2);
title('sin(50*x)')

subplot(4,2,3);
y3 = 50.*sin(x);
plot(x,y3);
title('50*sin(x)')

subplot(4,2,4);
y4 = sin(x)+50;
plot(x,y4);
title('sin(x)+50')

subplot(4,2,5);
y5 = sin(x+50);
plot(x,y5);
title('sin(x+50)')

subplot(4,2,6);
y6 = 50.*sin(50.*x);
plot(x,y6);
title('50*sin(50*x)')

subplot(4,2,7);
y7 = x.*sin(x);
plot(x,y7);
title('x*sin(x)')

subplot(4,2,8);
y8 = sin(x)./x;
plot(x,y8);
title('sin(x)/x')

%-----------------------------------
% Problem 4
%-----------------------------------

figure;
x = -20:20;

subplot(5,2,1);
y1 = sin(x);
plot(x,y1);
title('Problem 4: sin(x)')

subplot(5,2,2);
y2 = sin(50.*x);
plot(x,y2);
title('sin(50*x)')

subplot(5,2,3);
y3 = 50.*sin(x);
plot(x,y3);
title('50*sin(x)')

subplot(5,2,4);
y4 = sin(x)+50;
plot(x,y4);
title('sin(x)+50')

subplot(5,2,5);
y5 = sin(x+50);
plot(x,y5);
title('sin(x+50)')

subplot(5,2,6);
y6 = 50.*sin(50.*x);
plot(x,y6);
title('50*sin(50*x)')

subplot(5,2,7);
y7 = x.*sin(x);
plot(x,y7);
title('x*sin(x)')

subplot(5,2,8);
y8 = sin(x)./x;
plot(x,y8);
title('sin(x)/x')

subplot(5,2,9);
y9 = y1+y2+y3+y4+y5+y6+y7+y8;
plot(x,y9);
title('Sum of all')

%-----------------------------------
% Problem 5
%-----------------------------------

figure;

z = randn(1,41);

subplot(5,2,1);
y10 = z;
plot(x,y10);
title('Problem 5: z')

subplot(5,2,2);
y11 = z+x;
plot(x,y11);
title('z+x')

subplot(5,2,3);
y12 = z+sin(x);
plot(x,y12);
title('z+sin(x)')

subplot(5,2,4);
y13 = z.*sin(x);
plot(x,y13);
title('z*sin(x)')

subplot(5,2,5);
y14 = x.*sin(z);
plot(x,y14);
title('x*sin(z)')

subplot(5,2,6);
y15 = sin(x+z);
plot(x,y15);
title('sin(x+z)')

subplot(5,2,7);
y16 = z.*sin(50.*x);
plot(x,y16);
title('z*sin(50*x)')

subplot(5,2,8);
y17 = sin(x+50.*z);
plot(x,y17);
title('sin(x+50*z)')

subplot(5,2,9);
y18 = sin(x)./z;
plot(x,y18);
title('sin(x)/z')

subplot(5,2,10);
y19 = y11+y12+y13+y14+y15+y16+y17+y18;
plot(x,y19);
title('Sum of all')

%-----------------------------------
% Problem 6
%-----------------------------------

figure;

z = rand(1,41);

subplot(5,2,1);
y20 = z;
plot(x,y20);
title('Problem 6: z')

subplot(5,2,2);
y21 = z+x;
plot(x,y21);
title('z+x')

subplot(5,2,3);
y22 = z+sin(x);
plot(x,y22);
title('z+sin(x)')

subplot(5,2,4);
y23 = z.*sin(x);
plot(x,y23);
title('z*sin(x)')

subplot(5,2,5);
y24 = x.*sin(z);
plot(x,y24);
title('x*sin(z)')

subplot(5,2,6);
y25 = sin(x+z);
plot(x,y25);
title('sin(x+z)')

subplot(5,2,7);
y26 = z.*sin(50.*x);
plot(x,y26);
title('z*sin(50*x)')

subplot(5,2,8);
y27 = sin(x+50.*z);
plot(x,y27);
title('sin(x+50*z)')

subplot(5,2,9);
y28 = sin(x)./z;
plot(x,y28);
title('sin(x)/z')

subplot(5,2,10);
y29 = y21+y22+y23+y24+y25+y26+y27+y28;
plot(x,y29);
title('Sum of all')

%-----------------------------------
% Problem 7
%-----------------------------------

figure;

z = randn(1,10000);

r1 = z; 
r2 = 2.*z; 
r3 = 4.*z; 
r4 = 16.*z;

subplot(2,2,1);
histogram(r1);
title('Problem 7: X(0,1) Gaussian Random')

subplot(2,2,2);
histogram(r2);
title('X(0,4) Gaussian Random')

subplot(2,2,3);
histogram(r3);
title('X(0,16) Gaussian Random')

subplot(2,2,4);
histogram(r4);
title('X(0,256) Gaussian Random')

%-----------------------------------
% Problem 8
%-----------------------------------

figure;

r6 = 10 + z;
r7 = 20 + 2.*z;
r8 = -10 + z;
r9 = -20 + 2.*z;

subplot(2,2,1);
histogram(r6);
title('Problem 8: X(10,1) Gaussian Random')

subplot(2,2,2);
histogram(r7);
title('X(20,4) Gaussian Random')

subplot(2,2,3);
histogram(r8);
title('X(-10,1) Gaussian Random')

subplot(2,2,4);
histogram(r9);
title('X(-20,4) Gaussian Random')

%-----------------------------------
% Problem 9
%-----------------------------------

figure;

z = sqrt(12).*(rand(1,10000)-0.5);

r11 = z;
r21 = 2.*z;
r31 = 4.*z;
r41 = 16.*z;

subplot(2,2,1);
histogram(r11);
title('Problem 9: mean 0, variance 1')

subplot(2,2,2);
histogram(r21);
title('mean 0, variance 4')

subplot(2,2,3);
histogram(r31);
title('mean 0, variance 16')

subplot(2,2,4);
histogram(r41);
title('mean 0, variance 256')

%-----------------------------------
% Problem 10
%-----------------------------------

figure;

z = sqrt(12).*(rand(1,10000)-0.5);

r61 = z + 10;
r71 = 2.*z +20;
r81 = z - 10;
r91 = 2.*z -20;

subplot(2,2,1);
histogram(r61);
title('Problem 10: mean 10, variance 1')

subplot(2,2,2);
histogram(r71);
title('mean 20, variance 4')

subplot(2,2,3);
histogram(r81);
title('mean -10, variance 1')

subplot(2,2,4);
histogram(r91);
title('mean -20, variance 4')
