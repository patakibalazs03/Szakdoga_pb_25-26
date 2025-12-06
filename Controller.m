%% Display step response
Bemenet{1} = out.Alapjel.Time;
Bemenet{2} = squeeze(out.Alapjel.Data);
Bal_kerek{1} = out.Bal_kerek.Time;
Bal_kerek{2} = squeeze(out.Bal_kerek.Data);
Jobb_kerek{1} = out.Jobb_kerek.Time;
Jobb_kerek{2} = squeeze(out.Jobb_kerek.Data);
% Bal_beavatkozo{1} = out.Bal_beavatkozo.Time;
% Bal_beavatkozo{2} = squeeze(out.Bal_beavatkozo.Data);
% Jobb_beavatkozo{1} = out.Jobb_beavatkozo.Time;
% Jobb_beavatkozo{2} = squeeze(out.Jobb_beavatkozo.Data);

figure(1);
hold on;
grid on;
title("Szabályozás szimuláció");
xlabel("Idő [s]");
plot(Bemenet{1},Bemenet{2});
plot(Bal_kerek{1},Bal_kerek{2});
plot(Jobb_kerek{1},Jobb_kerek{2});
ylabel("Sebesség [m/s]");
legend('Alapjel','Baloldali kerék sebesség', ...
    'Jobboldali kerék sebesség');
legend("boxoff");
legend(location="southwest");
hold off;

%% Signal processing
clc
refsignal = max(Bemenet{2});
felfutoelt1 = Bal_kerek{1}(251:301,1);
felfutoeld1 = Bal_kerek{2}(251:301,1);
felfutoelt2 = Jobb_kerek{1}(251:301,1);
felfutoeld2 = Jobb_kerek{2}(251:301,1);

idx = 1;
while felfutoeld1(idx) < 0.294
    idx = idx +1;
end
settingtime1 = felfutoelt1(idx)-5;
 
idx = 1;
while felfutoeld2(idx) < 0.294
    idx = idx +1;
end
settingtime2 = felfutoelt2(idx)-5;

fprintf("A két beállási idő (bal, jobb): %.2f,%.2f [s]\n",settingtime1,settingtime2);

overshoot1 = (max(Bal_kerek{2})-refsignal)/refsignal*100;
overshoot2 = (max(Jobb_kerek{2})-refsignal)/refsignal*100;

fprintf("Bal oldali kerék túllövés %.4f százalék, Jobb oldali kerék túllövés %.4f százalék\n",overshoot1,overshoot2);

%% Display actuator signal
figure(2);
hold on;
grid on;
title("Szabályozás szimuláció: beavatkozójel");
xlabel("Idő [s]");
plot(Bal_beavatkozo{1},Bal_beavatkozo{2});
plot(Jobb_beavatkozo{1},Jobb_beavatkozo{2});
ylabel("Beavatkozójel Nagysága [-]");
legend('Baloldali kerák beavatkozójel', ...
    'Jobboldali kerék beavatkozójel');
legend("boxoff");
legend(location="southwest");
hold off;

%% Display step response \w actuator signal
% figure(3);
% hold on;
% grid on;
% title("Szabályozás szimuláció");
% xlabel("Idő [s]");
% yyaxis("left");
% plot(Bemenet{1},Bemenet{2});
% plot(Bal_kerek{1},Bal_kerek{2});
% plot(Jobb_kerek{1},Jobb_kerek{2});
% ylabel("Sebesség [m/s]");
% yyaxis("right");
% plot(Bal_beavatkozo{1},Bal_beavatkozo{2});
% plot(Jobb_beavatkozo{1},Jobb_beavatkozo{2});
% ylabel("Beavatkozójel Nagysága [-]");
% legend('Alapjel','Baloldali kerék sebesség', ...
%     'Jobboldali kerék sebesség','Baloldali kerák beavatkozójel', ...
%     'Jobboldali kerék beavatkozójel');
% legend("boxoff");
% legend(location="southwest");
% hold off;