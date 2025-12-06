clear;
load("Identifikacio_eredmenye.mat");
clc;
nl_x1 = sort(nlhw1.InputNonlinearity.BreakPoints(1,:));
nl_y1 = sort(nlhw1.InputNonlinearity.BreakPoints(2,:));
% nl_x12 = nlhw1.InputNonlinearity.BreakPoints(1,:);
% nl_y12 = nlhw1.InputNonlinearity.BreakPoints(2,:);
nl_x2 = sort(nlhw2.InputNonlinearity.BreakPoints(1,:));
nl_y2 = sort(nlhw2.InputNonlinearity.BreakPoints(2,:));

nl_x1(1,6) = 0;
nl_y1(1,6) = 0;
nl_y1(1,5) = -1*nl_y1(1,5);

nl_x2(1,6) = 0;
nl_y2(1,6) = 0;
nl_y2(1,5) = -1*nl_y2(1,5);

% figure(1)
% hold on;
% grid on;
% plot(nl_x1,nl_y1);
% plot(nl_x2,nl_y2);
% title("Bemeneti Nemlinearitás");
% xlabel("Bemenet [%]");
% ylabel("Nemlinearitás");
% legend("Bal oldali kerék nemlinearitása","Jobb oldali kerék nemlinearitása")
% % plot(nl_x12,nl_y12);
% % legend("javitott","eredeti");
% hold off;

%% State space linear models

linear_sys_ss1 = ss(nlhw1.LinearModel); %This is a discrete time system
                                        %so no need to convert it
linear_sys_ss2 = ss(nlhw2.LinearModel);

P1 = pole(linear_sys_ss1);
P2 = pole(linear_sys_ss2);

% Pole placement
Ts = 0.02;

% step(linear_sys_ss1,linear_sys_ss2);
%% Pole setting in continouos time
w01 = 9;
w02 = 7.5;
xi1 = 1.95;
xi2 = 1.75;
% lin2 = tf(w02^2,[1 2*w02*xi w02^2]);
% lin1 = tf(w01^2,[1 2*w01*xi w01^2]);
% [yOut,tOut]= step(lin1);
% 
% overshoot = (max(yOut)-1)/1*100;

%%

s1 = -w01*xi1+1i*w01*sqrt(1-xi1^2);
s1c = conj(s1);
s2 = -w02*xi2+1i*w02*sqrt(1-xi2^2);
s2c = conj(s2);
z1 = exp(s1*Ts);
z2 = exp(s2*Ts);
z1c = exp(s1c*Ts);
z2c = exp(s2c*Ts);
scinf1 = 3*real(s1);
scinf2 = 3*real(s2);
zcinf1 = exp(scinf1*Ts);
zcinf2 = exp(scinf1*Ts);
soinf1 = 5*real(s1);
soinf2 = 5*real(s2);
zoinf1 = exp(soinf1*Ts);
zoinf2 = exp(soinf2*Ts);

% obsv(linear_sys_ss1.A,linear_sys_ss1.C);
% ctrb(linear_sys_ss1.A,linear_sys_ss1.B);
% obsv(linear_sys_ss2.A,linear_sys_ss2.C);
% ctrb(linear_sys_ss2.A,linear_sys_ss2.B);

Phi1 = linear_sys_ss1.A;
Phi2 = linear_sys_ss2.A;
Gamma1 = linear_sys_ss1.B;
Gamma2 = linear_sys_ss2.B;
C1 = linear_sys_ss1.C;
C2 = linear_sys_ss2.C;

% State gain

K1 = acker(Phi1,Gamma1,[z1 z1c zoinf1]);
K2 = acker(Phi2,Gamma2,[z2 z2c zoinf2]);

% State estimation

% Gt1 = acker(Phi1',Phi1'*C1',[zoinf1 zoinf1 zoinf1]');
% Gt2 = acker(Phi2',Phi2'*C2',[zoinf2 zoinf2 zoinf2]');
% 
% G1 = Gt1';
% G2 = Gt2';
% 
% F1 = Phi1-G1*C1*Phi1;
% F2 = Phi2-G2*C2*Phi2;
% 
% H1 = Gamma1-G1*C1*Gamma1;
% H2 = Gamma2-G2*C2*Gamma2;

% Reference signal gain

N1 = inv([Phi1-eye(3) Gamma1; C1 0]) * [0 0 0 1]';
N2 = inv([Phi2-eye(3) Gamma2; C2 0]) * [0 0 0 1]';
Nx1 = N1(1:3);
Nx2 = N2(1:3);
Nu1 = N1(4);
Nu2 = N2(4);

% load estimator

Phit1 = [Phi1 Gamma1; zeros(1,3) 1];   %t for tilde
Phit2 = [Phi2 Gamma2; zeros(1,3) 1];

Gammat1 = [Gamma1; zeros(1,1)];
Gammat2 = [Gamma2; zeros(1,1)];

Ct1 = [C1 1];
Ct2 = [C2 1];

Gtilde1 = acker(Phit1',Phit1'*Ct1',[zcinf1 zcinf1 zcinf1 zcinf1]')';
Gtilde2 = acker(Phit2',Phit2'*Ct2',[zcinf2 zcinf2 zcinf2 zcinf2]')';

Ft1=Phit1-Gtilde1*Ct1*Phit1;
Ft2=Phit2-Gtilde2*Ct2*Phit2;
Ht1=Gammat1-Gtilde1*Ct1*Gammat1;
Ht2=Gammat2-Gtilde2*Ct2*Gammat2;