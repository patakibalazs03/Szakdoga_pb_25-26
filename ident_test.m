nl_x1 = sort(nlhw1.InputNonlinearity.BreakPoints(1,:));
nl_y1 = sort(nlhw1.InputNonlinearity.BreakPoints(2,:));
% nl_x12 = nlhw1.InputNonlinearity.BreakPoints(1,:)
% nl_y12 = nlhw1.InputNonlinearity.BreakPoints(2,:)
nl_x2 = sort(nlhw2.InputNonlinearity.BreakPoints(1,:));
nl_y2 = sort(nlhw2.InputNonlinearity.BreakPoints(2,:));

% figure(1)
% hold on;
% plot(nl_x1,nl_y1);
% plot(nl_x12,nl_y12);
% legend("javitott","eredeti");
% hold off;

%% State space linear models

linear_sys_ss1 = ss(nlhw1.LinearModel); %This is a discrete time system
                                        %so no need to convert it
linear_sys_ss2 = ss(nlhw2.LinearModel);

P1 = pole(linear_sys_ss1);
P2 = pole(linear_sys_ss2);

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

K1 = acker(Phi1,Gamma1,P1);
K2 = acker(Phi2,Gamma2,P2);

% Trying yout different values for zoinf, zcinf
Ts = 0.02;
w0 = 100;
soinf = -5 *w0;
scinf = -3 *w0;
zoinf = exp(soinf*Ts);
zcinf = exp(scinf*Ts);

Gt1 = acker(Phi1',Phi1'*C1',[zoinf zoinf zoinf]');
Gt2 = acker(Phi2',Phi2'*C2',[zoinf zoinf zoinf]');

G1 = Gt1';
G2 = Gt2';

F1 = Phi1-G1*C1*Phi1;
F2 = Phi2-G2*C2*Phi2;

H1 = Gamma1-G1*C1*Gamma1;
H2 = Gamma2-G2*C2*Gamma2;

N1 = inv([Phi1-eye(3) Gamma1; C1 0]) * [0 0 0 1]';
N2 = inv([Phi2-eye(3) Gamma2; C2 0]) * [0 0 0 1]';

Nx1 = N1(1:3);
Nx2 = N2(1:3);
Nu1 = N1(4);
Nu2 = N2(4);
