load('tavmeromeres.mat');

meres{1} = data{3}{8}.Values.Time;
meres{2} = squeeze(data{3}{8}.Values.Data);

p = 2000;

while max(meres{2}) > p
    [i,idx] = max(meres{2});
    meres{2}(idx) = p;
end

figure(1);
hold on;
grid on;
plot(meres{1},meres{2},'r-');
title('Távolságmérő valós idejű mérés');
xlabel("Idő [s]");
ylabel("Mért távolság [mm]");
hold off;