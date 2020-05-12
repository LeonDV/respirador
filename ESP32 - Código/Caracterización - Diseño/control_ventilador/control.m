clc; close all;

s = tf('s');
Ref = 30;
Gain = 27*6/(4095-2400);
tao = 0.01;
Gvel = Gain/(tao*s+1);
Gpos = Gvel/s;

Ts = 7.5e-3;

Gdpos = c2d(Gpos,Ts);

sisotool(Gdpos)

z = tf('z',Ts);

K = 2000*(z-0.7165)*(z-0.9)/(z*(z-1));
%K = 2000;
lim = 4095 - 2400;
t = 0:Ts:0.3;
ref = Ref/0.3*t;
y = lsim(feedback(Gdpos*K,1),ref,t);
e = (ref' - y);
u = K*e;

error = sum(e);

subplot(311)
plot(t,y,t,ref)
title('Seguimiento')
legend('pos','ref')
ylabel('[º]')
subplot(312)
plot(t,e)
title('Error de seguimiento')
ylabel('[º]')
subplot(313)
stairs(t,u)
hold on
plot(t,lim*ones(1,length(t)))
legend('PWM','Lim')
title('Acción de control')
ylabel('PWM')
xlabel('t [s]')