clc; close all;

s = tf('s');

%ret = exp(-0.01*s);
ret = 1;

Gvel = 0.01328*ret/(0.012*s+1);
Gpos = Gvel/s;

Ts = 4e-3;

Gdpos = c2d(Gpos,Ts);

% sisotool(Gdpos)

z = tf('z',Ts);

%K = 5000*(z-0.7165)*(z-0.9)/(z*(z-1));
K = 2000;
lim = 4095 - 1420;
t = 0:Ts:0.3;
ref = 30/0.3*t;
y = lsim(feedback(Gdpos*K,1),ref,t);
e = (ref' - y);
u = K*e;

error = sum(e);

subplot(311)
plot(t,y,t,ref)
title('Seguimiento')
legend('pos','ref')
ylabel('[�]')
subplot(312)
plot(t,e)
title('Error de seguimiento')
ylabel('[�]')
subplot(313)
stairs(t,u)
hold on
plot(t,lim*ones(1,length(t)))
legend('PWM','Lim')
title('Acci�n de control')
ylabel('PWM')
xlabel('t [s]')