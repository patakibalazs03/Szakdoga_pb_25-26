%% Display
Bemenet{1} = out.Alapjel.Time;
Bemenet{2} = squeeze(out.Alapjel.Data);
Bal_kerek{1} = out.Bal_kerek.Time;
Bal_kerek{2} = squeeze(out.Bal_kerek.Data);
Jobb_kerek{1} = out.Jobb_kerek.Time;
Jobb_kerek{2} = squeeze(out.Jobb_kerek.Data);
Bal_beavatkozo{1} = out.Bal_beavatkozo.Time;
Bal_beavatkozo{2} = squeeze(out.Bal_beavatkozo.Data);
Jobb_beavatkozo{1} = out.Jobb_beavatkozo.Time;
Jobb_beavatkozo{2} = squeeze(out.Jobb_beavatkozo.Data);

figure(1);
hold on;
grid on;
title("Szabályozás szimuláció");
xlabel("Idő [s]");
yyaxis("left");
plot(Bemenet{1},Bemenet{2});
plot(Bal_kerek{1},Bal_kerek{2});
plot(Jobb_kerek{1},Jobb_kerek{2});
ylabel("Sebesség [m/s]");
yyaxis("right");
plot(Bal_beavatkozo{1},Bal_beavatkozo{2});
plot(Jobb_beavatkozo{1},Jobb_beavatkozo{2});
ylabel("Beavatkozójel Nagysága [-]");
legend('Alapjel','Baloldali kerék sebesség', ...
    'Jobboldali kerék sebesség','Baloldali kerák beavatkozójel', ...
    'Jobboldali kerék beavatkozójel');
legend("boxoff");
legend(location="southwest");
hold off;

%% Signal processing

settingtime1 = 1;