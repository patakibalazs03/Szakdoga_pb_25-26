Ido = Bemenet.Time;
Input = Bemenet.Data;
Ido1 = Kerek1_elore.Time;
Ido2 = Kerek2_elore.Time;
Output1 = squeeze(Kerek1_elore.Data);
Output2 = squeeze(Kerek2_elore.Data);

% Ido = linspace(0,25);
% Ido1 = Ido; Ido2 = Ido;
% Input = sqrt(Ido);
% Output1 = 2*Ido;
% Output2 = Ido;

figure(1);
hold on;
title("Identifikáció");
yyaxis left;
plot(Ido,Input);
xlabel("Idő [s]");
ylabel("Bemeneti jel [%]");
yyaxis right;
plot(Ido1,Output1,"-.r");
plot(Ido2,Output2,"-r");
ylabel("Kerekek sebessége [rad/s]");
legend('Bemenet [%]','Bal oldali kerék sebessége [rad/s]','Jobb oldali kerék sebessége [rad/s]');
legend("boxoff");
legend(location="northwest");
hold off;
