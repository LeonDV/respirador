clc; close all;

load('datos_1.mat')

t = datos1(:,1);
ref = datos1(:,5);
pos = datos1(:,6);
vel = datos1(:,7);

Tf = 749;

subplot(211)
plot(t(1:Tf),ref(1:Tf),t(1:Tf),pos(1:Tf))
title('POSICIÓN')
ylabel('[º]')
legend('ref','pos')
grid on
subplot(212)
plot(t(1:Tf),vel(1:Tf))
title('VELOCIDAD')
ylabel('[rpm]')
xlabel('t [ms]')
grid on