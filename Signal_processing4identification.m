Ido = Bemenet_elore.Time;
Input = Bemenet_elore.Data;
Ido1 = Kerek1_elore.Time;
Ido2 = Kerek2_elore.Time;
Output1 = squeeze(Kerek1_elore.Data);
Output2 = squeeze(Kerek2_elore.Data);

figure(1);
hold on;
plot(Ido,Input);
plot(Ido1,Output1);
plot(Ido2,Output2);
xlabel("Idő [s]");
ylabel("Jel nagysága/kerék sebesség [rad/s]")
hold off;
