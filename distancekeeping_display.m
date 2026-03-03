Ido = Tavolsagmeres_er{1}.Values.Time;
Tav = squeeze(Tavolsagmeres_er{1}.Values.Data);
Bal_seb = squeeze(Tavolsagmeres_er{2}.Values.Data);
Jobb_seb = squeeze(Tavolsagmeres_er{3}.Values.Data);

idxl = 1;
while Ido(idxl) < 50
    idxl = idxl + 1;
end

idxu = 1;
while Ido(idxu) < 95
    idxu = idxu + 1;
end

Ido = Ido(idxl:idxu,1);
Tav = Tav(idxl:idxu,1);
Bal_seb = Bal_seb(idxl:idxu,1);
Jobb_seb = Jobb_seb(idxl:idxu,1);

figure(1);
hold on;
grid on;
plot(Ido,Tav);
xlabel("Idő [s]");
ylabel("Mért távolság [m]");
title("Távolságtartás: mért távolság");
hold off;

figure(2);
hold on;
grid on;
plot(Ido,Bal_seb);
plot(Ido,Jobb_seb);
title("Távolságmérés: mért kerék sebesség");
xlabel("Idő [s]");
ylabel("Kerekek sebessége [m/s]");
legend("Baloldali kerék sebessége", "Jobboldali kerék sebessége");
hold off;