Diameter = 0.065; %m
Radius = Diameter/2;

%% Identification for forward movement

Ido = Bemenet.Time;
Input = Bemenet.Data;
Ido1 = Kerek1_elore.Time;
Ido2 = Kerek2_elore.Time;
Output1 = Radius * squeeze(Kerek1_elore.Data);
Output2 = Radius * squeeze(Kerek2_elore.Data);

% Clearing the oddities
max_finite1 = max(Output1(isfinite(Output1)));
max_finite2 = max(Output2(isfinite(Output2)));

Output1(isinf(Output1)) = max_finite1;
Output2(isinf(Output2)) = max_finite2;

figure(1);
hold on;
title("Identifikáció közös módusú, előre történő hajtás esetén");
yyaxis left;
plot(Ido,Input);
xlabel("Idő [s]");
ylabel("Bemeneti jel [%]");
yyaxis right;
plot(Ido1,Output1,"-.");
plot(Ido2,Output2,"-r");
ylabel("Kerekek sebessége [m/s]");
legend('Bemenet [%]','Bal oldali kerék sebessége [m/s]','Jobb oldali kerék sebessége [m/s]');
legend("boxoff");
legend(location="northwest");
hold off;

%% Identification for backwards movement

%Ido = Bemenet.Time;
Input2 = -Bemenet.Data;
%Ido1 = Kerek1_hatra.Time;
%Ido2 = Kerek2_hatra.Time;
Output3 = -Radius * squeeze(Kerek1_hatra.Data);
Output4 = -Radius * squeeze(Kerek2_hatra.Data);

max_finite3 = max(Output3(isfinite(Output3)));
max_finite4 = max(Output4(isfinite(Output4)));
Output3(isinf(Output3)) = max_finite3;
Output4(isinf(Output4)) = max_finite4;

figure(2);
hold on;
title("Identifikáció közös módusú, hátra történő hajtás esetén");
yyaxis left;
plot(Ido,Input2);
xlabel("Idő [s]");
ylabel("Bemeneti jel [%]");
yyaxis right;
plot(Ido1,Output3,"-.");
plot(Ido2,Output4,"-r");
ylabel("Kerekek sebessége [m/s]");
legend('Bemenet [%]','Bal oldali kerék sebessége [m/s]','Jobb oldali kerék sebessége [m/s]');
legend("boxoff");
legend(location="northwest");
hold off;

%% Identification for backwards and forwards movement

Ido_all = [Ido, Ido+40];
Input_all = [Input, -Input];
Output_bal = [Output1, -Output3];
Output_jobb = [Output2, -Output4];

figure(3);
hold on;
title("Identifikáció közös módusú hajtás esetén");
yyaxis left;
plot(Ido_all,Input_all);
xlabel("Idő [s]");
ylabel("Bemeneti jel [%]");
yyaxis right;
plot(Ido_all,Output_bal,"-.");
plot(Ido_all,Output_jobb,"-r");
ylabel("Kerekek sebessége [m/s]");
legend('Bemenet előremenetben [%]','Bemenet hátramenetben [%]','Bal oldali kerék sebessége [m/s]','Jobb oldali kerék sebessége [m/s]');
legend("boxoff");
legend(location="northwest");
hold off;

%% New method

Ido = Bemenet.Time;
Input = Bemenet.Data;
Ido1 = Kerek1.Time;
Ido2 = Kerek2.Time;
Output1 = -Radius * squeeze(Kerek1.Data);
Output2 = Radius * squeeze(Kerek2.Data);

max_finite1 = max(Output1(isfinite(Output1)));
max_finite2 = max(Output2(isfinite(Output2)));

Output1(isinf(Output1)) = max_finite1;
Output2(isinf(Output2)) = max_finite2;

figure(4);
hold on;
title("Identifikáció közös módusú hajtás esetén");
yyaxis left;
plot(Ido,Input);
xlabel("Idő [s]");
ylabel("Bemeneti jel [%]");
yyaxis right;
plot(Ido1,Output1,"-.");
plot(Ido2,Output2,"-r");
ylabel("Kerekek sebessége [m/s]");
legend('Bemenet [%]','Bal oldali kerék sebessége [m/s]','Jobb oldali kerék sebessége [m/s]');
legend("boxoff");
legend(location="northwest");
hold off;


