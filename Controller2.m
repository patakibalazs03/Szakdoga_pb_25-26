%% Testing controller on the robot

Ido = simout.Time;
Alapjel = simout.Data(:,1);
Bal_beav_sz = simout.Data(:,2);
Bal_beav = simout.Data(:,3);
Jobb_beav_sz = simout.Data(:,4);
Jobb_beav = simout.Data(:,5);
Bal_seb = simout.Data(:,6);
Jobb_seb = simout.Data(:,7);

figure(1);
hold on;
grid on;
title("Szabályozás a roboton: sebesség");
xlabel("Idő [s]");
plot(Ido,Alapjel);
plot(Ido,Bal_seb);
plot(Ido,Jobb_seb);
ylabel("Sebesség [m/s]");
legend('Alapjel','Baloldali kerék sebesség', ...
    'Jobboldali kerék sebesség');
legend("boxoff");
legend(location="southwest");
hold off;

%% Actuator signal in percent

figure(2);
hold on;
grid on;
title("Szabályozás a roboton: beavatkozójel");
xlabel("Idő [s]");
plot(Ido,Bal_beav_sz);
plot(Ido,Jobb_beav_sz);
ylabel("Beavatkozójel nagysága [PWM%]");
legend('Baloldali kerék beavatkozójel', ...
    'Jobboldali kerék beavatkozójel');
legend("boxoff");
legend(location="southwest");
hold off;