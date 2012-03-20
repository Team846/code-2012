function mvmass(Kdg);
% Move a mass by applying a force F. 
% Damping/Friction = b in lb*sec/ft
% spring constant = k in lb/ft
% Mx'' + bx' +kx = F
% X(s)/F(s) = 1 / (Ms^2+bs+k)

M=143.4; % lbs
Kp = 0.75*M;
Ki = 0.1*M;
Kd = 1.75*M;
b = 0.000010;
k = 0.000020;
num=[Kd Kp Ki];
den=[M b*M+Kd k*M+Kp Ki];
figure(3);
grid on;
bode(num,den);
%nyquist(num,den);
%grid on;
%pause;
Tstep = 0.02; % 50 Hz
Tstop = 50;
t=0:Tstep:Tstop;
su = max(size(t));
figure(2);
grid on;
step(num,den,t);
sys=tf(num,den);
u = t;
u(1:fix(5*su/Tstop)) = 0;
u(fix(5*su/Tstop):fix(25*su/Tstop)) = 3;
u(fix(25*su/Tstop):su) = 0;
figure(1);
grid on;
lsim(sys,u,t);


