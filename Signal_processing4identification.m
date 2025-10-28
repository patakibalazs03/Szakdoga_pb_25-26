Ido = Bemenet.Time;
Input = Bemenet.Data;
Ido1 = Kerek1_elore.Time;
Ido2 = Kerek2_elore.Time;
Output1 = squeeze(Kerek1_elore.Data);
Output2 = squeeze(Kerek2_elore.Data);

figure(1);
hold on;
plot(Ido,Input);
plot(Ido1,Output1);
plot(Ido2,Output2);
legend('Bemenet [%]','Bal oldali kerék sebessége [rad/s]','Jobb oldali kerék sebessége [rad/s]');
xlabel("Idő [s]");
ylabel("Bemeneti jel [%]/Kerék sebesség [rad/s]");
legend("boxoff");
legend(location="northwest");
hold off;
