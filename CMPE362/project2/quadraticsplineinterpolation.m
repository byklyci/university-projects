x = [265 400 500 700 950 1360 2080 2450 2940];
y = [1025 1400 1710 2080 2425 2760 3005 2850 2675];

A = zeros(24);

% spline pass through start point
for i=1:8
    A(i,(i-1)*3+1) = x(i).*x(i);
    A(i,(i-1)*3+2) = x(i);
    A(i,(i-1)*3+3) = 1;
end

% spline pass through end point
for i=2:9
    A(i+7,(i-2)*3+1) = x(i).*x(i);
    A(i+7,(i-2)*3+2) = x(i);
    A(i+7,(i-2)*3+3) = 1;
end

% slope at intersection of splines is equal to each other
for i=2:8
    A(i+15,(i-2)*3+1)= 2*x(i);
    A(i+15,(i-2)*3+2)= 1;
    A(i+15,(i-2)*3+4)= -2*x(i);
    A(i+15,(i-2)*3+5)= -1;
end

% first spline has 0 as coefficient for the x^2
A(24,1) = 1;

B = zeros(24,1);

for i=1:8
    B(i) = y(i);
end

for i=2:9
    B(i+7) = y(i);
end

Res = linsolve(A,B);

% plot each spline
for i=1:8
    xspline = x(i):1:x(i+1);
    yspline = Res((i-1)*3+1).*xspline.^2 + Res((i-1)*3+2).*xspline + Res((i-1)*3+3);
    plot(xspline,yspline);
    hold on;
end

plot(x,y,'o','MarkerEdgeColor','r'); % plot real points

yl = ylim; % Get current limits.
ylim([0, yl(2)]); % Replace lower limit only with a y of 0.

xl = xlim; % Get current limits.
xlim([0, xl(2)]); % Replace lower limit only with a x of 0.
