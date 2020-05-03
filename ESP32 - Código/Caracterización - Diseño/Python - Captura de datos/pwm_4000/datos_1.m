clc; close all;

load('datos_1.mat')

t = datos1(:,1);
ref = datos1(:,5);
pos = datos1(:,6);
vel = datos1(:,7);

subplot(211)
plot(t,ref,t,pos)
title('POSICIÓN')
ylabel('[º]')
legend('ref','pos')
subplot(212)
plot(t,vel)
title('VELOCIDAD')
ylabel('[rpm]')
xlabel('t [ms]')