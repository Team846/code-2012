function robo(param)
% Simulate 846 robot controller and plot results
% Author: Alex
%
% Rev. log
% March 19, 2012 - Initial release
%
%
% Here are a few parameters of the robot.
% 120 lb + bumpers + battery
% 13 lb battery
% 10.4 lb Bumpers
% 143.4 lb total
% 12 degrees of slop with 8" diam wheels. Slop is before 2:1 reduction.
% This results in 8*pi*12/360 = 0.84 inches of longitudinal slop 
% 334 lb-in of torque to start turning
% Robot gearing specs @ 40 amps
% 15.7 	ft/s
% 75.94
% 	lbs
%
% 6.1 	ft/s
% 194.4
% 	lbs
%
% Pushing force at @ 60 amps (max continuous current)
% High gear
% 113.9
% Low Gear
% 291.6
%
% CG 16-20 in off ground
% CG 11-15in from front
% Width = 23.5
% Length = 22.5
%
% Cof ranges from 0.9-1.3 on carpet
%
% hg clone ssh://hg@bitbucket.org/brianaxelrod/frc_gazebo
%
% Bridge specs:
% 7 in pivot center to pivot
%
% Wood Bridge
% 62 lb
% Real Bridge according to unofficial model
% 251 lb
% Real Bridge according to official model (does not include polycarb (18 lb))
% 171 lb
% <inertia ixx="41678*lbin2_kgm2" ixy="1*lbin2_kgm2" ixz="-38*lbin2_kgm2" iyy="134691*lbin2_kgm2" iyz="0.01" izz="93678*lbin2_kgm2"/>
% (lb*in^2)

% Robot state
robo.mass = 143.4;
robo.speedingear1 =  6.1; % ft/s
robo.speedingear2 = 15.7; % ft/s
robo.b = 10; % lb * sec / feet
robo.throttle = 0;
robo.jerk = 0;
robo.acc = 0;
robo.vel = 0;
robo.pos = 0;
robo.angle = 0;
robo.gravityforce = 0;
robo.damper = 0;

st.prev_int = 0;
st.prev_td = 0;
st.prev_err = 0;
st.prev_ti = 0;
st.PIDposition = 0;
st.PIDvelocity = 0;
st.PIDposveloc = 0;
st.resi = 0;
st.resd = 0;
st.idx = 0;

Tstep = 0.02; % 50 Hz
Tstop = 50;
% Input trajectory
t=0:Tstep:Tstop;
u=0:Tstep:Tstop;
su = max(size(u));
timelog = zeros(su,1);
throttlelog = zeros(su,1);
acceleralog = zeros(su,1);
velocitylog = zeros(su,1);
positionlog = zeros(su,1);
auxlog = zeros(su,1);
u(1:fix(su*5/Tstop)) = 0;
u(fix(su*5/Tstop):fix(su*25/Tstop)) = 3;
u(fix(su*25/Tstop):su) = 0;
% plot(t,u);

if 0
  % START POSITION CONTROL SIMULATION
  idx = 0;
  for t=0:Tstep:Tstop
    idx = idx+1;
    st.idx = idx;
    % Update robot state
    Error = u(idx) - robo.pos;
    st = PIDposition(Tstep,st,Error);
    Throttle = st.PIDposition;
    
    % APPLY THE GENTLE DRIVER(tm) FILTER
    prevthrottle = robo.throttle;
    throttlestep = Tstep * 0.5; % Scale it here, anyway you like
    if Throttle > prevthrottle
      prevthrottle = prevthrottle + throttlestep;
      if prevthrottle > Throttle
        prevthrottle = Throttle;
      end
    elseif Throttle < prevthrottle
      prevthrottle = prevthrottle - throttlestep;
      if prevthrottle < Throttle
        prevthrottle = Throttle;
      end
    end
    robo.throttle = prevthrottle;
    
    if 1
      if prevthrottle > 1
        prevthrottle = 1;
      elseif prevthrottle < -1
        prevthrottle = -1;
      end
    end
    % Update robot state
    % Compute and apply dampening force
    robo.damper = robo.vel * robo.b; % 10 lb * sec / feet
    robo.acc = accel(Tstep,prevthrottle,robo);
    robo.vel = robo.vel + veloc(Tstep,robo.acc);
    % Apply velocity limit imposed by motor, depending on gear position.
    if robo.vel > robo.speedingear1;
      robo.vel = robo.speedingear1;
    elseif robo.vel < -robo.speedingear1;
      robo.vel = -robo.speedingear1;
    end
    robo.pos = robo.pos + position(Tstep,robo.vel);
    timelog(idx) = t;
    throttlelog(idx) = prevthrottle;
    positionlog(idx) = robo.pos;
    velocitylog(idx) = robo.vel;
    acceleralog(idx) = robo.acc;
    % disp(['t: ' num2str(t) ' pos ' num2str(robo.pos)]);
  end
end

if 1
  % START POSITION CONTROL WITH CASCADED PID LOOPS
  idx = 0;
  for t=0:Tstep:Tstop
    idx = idx+1;
    st.idx = idx;
    % Update robot state
    PositError = u(idx) - robo.pos;
    st = PIDposveloc(Tstep,st,PositError);
    VelocitySetPoint = st.PIDposveloc;
    % Apply velocity limit, as in the cRio code
    VelocityLimit = 0.35; % Initially, unlimited velocity - cRio = 0.35
    if VelocitySetPoint > VelocityLimit
      VelocitySetPoint = VelocityLimit;
    elseif VelocitySetPoint < -VelocityLimit
      VelocitySetPoint = -VelocityLimit;
    end
    auxlog(idx) = VelocitySetPoint;
    VelocityError = VelocitySetPoint - robo.vel;
    st = PIDvelocity(Tstep,st,VelocityError);
    Throttle = st.PIDvelocity;
    
    % APPLY THE GENTLE DRIVER(tm) FILTER
    prevthrottle = robo.throttle;
    throttlestep = Tstep * 0.5; % Scale it here, anyway you like
    if Throttle > prevthrottle
      prevthrottle = prevthrottle + throttlestep;
      if prevthrottle > Throttle
        prevthrottle = Throttle;
      end
    elseif Throttle < prevthrottle
      prevthrottle = prevthrottle - throttlestep;
      if prevthrottle < Throttle
        prevthrottle = Throttle;
      end
    end
    robo.throttle = prevthrottle;
    
    if 1
      if prevthrottle > 1
        prevthrottle = 1;
      elseif prevthrottle < -1
        prevthrottle = -1;
      end
    end
    % Update robot state
    % Compute and apply dampening force
    robo.damper = robo.vel * robo.b; % 10 lb * sec / feet
    robo.acc = accel(Tstep,prevthrottle,robo);
    robo.vel = robo.vel + veloc(Tstep,robo.acc);
    % Apply velocity limit imposed by motor, depending on gear position.
    if robo.vel > robo.speedingear1;
      robo.vel = robo.speedingear1;
    elseif robo.vel < -robo.speedingear1;
      robo.vel = -robo.speedingear1;
    end
    robo.pos = robo.pos + position(Tstep,robo.vel);
    timelog(idx) = t;
    throttlelog(idx) = prevthrottle;
    positionlog(idx) = robo.pos;
    velocitylog(idx) = robo.vel;
    acceleralog(idx) = robo.acc;
    % disp(['t: ' num2str(t) ' pos ' num2str(robo.pos)]);
  end
end

figure(1);
clf;
plot(timelog,throttlelog);
grid on;
title('Throttle position');
figure(2);
clf;
plot(timelog,positionlog);
grid on;
title('Robot position');
figure(3);
clf;
plot(timelog,velocitylog);
grid on;
title('Robot velocity ');
figure(4);
clf;
plot(timelog,acceleralog);
grid on;
title('Robot acceleration ');
figure(5);
clf;
plot(timelog,auxlog);
grid on;
title('Velocity set point ');
return;


function accel = accel(Dt,throttle,robo)
prevacc = robo.acc;
% Use the maximum force in low gear. Change it if changing gears.
force = 291 * throttle - robo.gravityforce - robo.damper;
acceltarget = force/robo.mass;
% The following is to model the inertia of motors, gearbox, wheels
accstep = Dt * 1;  % Scale it here, anyway you like
if acceltarget > prevacc
  prevacc = prevacc + accstep;
  if prevacc > acceltarget
    prevacc = acceltarget;
  end
elseif acceltarget < prevacc
  prevacc = prevacc - accstep;
  if prevacc < acceltarget
    prevacc = acceltarget;
  end
end
accel = prevacc;

function Dv = veloc(Dt,accel)
Dv = accel * Dt;


function Dpos = position(Dt,veloc)
% The following is to model the slop, everytime we change direction
Dpos = veloc * Dt;


function st = PIDposition(Deltat,st,err)
pmass = 143.4; % used as a scale factor
scale = 0.003436;
Kp = 0.75 * pmass ;
Ki = 0.01 * pmass ;
Kd = 1.65 * pmass ;
st = integ(Deltat,st,err);
resi = st.resi;
st = deriv(Deltat,st,err);
resd = st.resd;
%disp(num2str(Kp*err + Ki*resi + Kd*resd));
st.PIDposition = (Kp*err + Ki*resi + Kd*resd) * scale;



function st = PIDposveloc(Deltat,st,err)
pmass = 143.4; % used as a scale factor
scale = 0.003436;
Kp = 1.0 * pmass; % cRio is using P = 0.05, I = 0, D = 0;
Ki = 0; %0.0 * pmass ;
Kd = 0; %0.0 * pmass ;
st = integ(Deltat,st,err);
resi = st.resi;
st = deriv(Deltat,st,err);
resd = st.resd;
%disp(num2str(Kp*err + Ki*resi + Kd*resd));
st.PIDposveloc = (Kp*err + Ki*resi + Kd*resd) * scale;


function st = PIDvelocity(Deltat,st,err)
pmass = 143.4; % used as a scale factor
scale = 0.003436;
Kp = 1.4 * pmass; % cRio is using P = 1.5, I = 0, D = 0;
% Ki = 0.05 * pmass ; % High quality velocity control
% Kd = 0.5 * pmass ; % High quality velocity control
Ki = 0; % Low quality velocity control
Kd = 0; % Low quality velocity control
st = integ(Deltat,st,err);
resi = st.resi;
st = deriv(Deltat,st,err);
resd = st.resd;
%disp(num2str(Kp*err + Ki*resi + Kd*resd));
st.PIDvelocity = (Kp*err + Ki*resi + Kd*resd) * scale;



function st = integ(DeltaT,st,err)
st.prev_int = st.prev_int * 0.999999;
st.prev_int = st.prev_int + err*DeltaT;
st.resi = st.prev_int;


function st = deriv(DeltaT,st,err)
DeltaErr = err - st.prev_err;
st.prev_err = err;
%disp(num2str(DeltaErr/DeltaT));
%disp(num2str(DeltaErr));
if DeltaT > 0
  st.resd = DeltaErr/DeltaT;
  % disp([num2str(err) ' ' num2str(st.res)]);
else
  st.resd = 0;
end
