clc; close all;

load('datos_1.mat')

t = datos1(:,1);
ref = datos1(:,5);
pos = datos1(:,6);
vel = datos1(:,7);

tf = 50;

subplot(211)
plot(t(1:tf),ref(1:tf),t(1:tf),pos(1:tf))
title('POSICIÓN')
ylabel('[º]')
legend('ref','pos')
grid on
subplot(212)
plot(t(1:tf),vel(1:tf))
title('VELOCIDAD')
ylabel('[rpm]')
xlabel('t [ms]')
grid on