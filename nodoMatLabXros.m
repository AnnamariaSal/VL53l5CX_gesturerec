rosinit('192.168.91.128'); %inserire indirizzo ip macchina 
sub = rossubscriber('/distance','DataFormat','struct');

while true
% Ricezione di messaggio dal topic
msg = receive(sub);

% Elaborazione dei dati del messaggio
dato=msg.Data;

vet=str2double(strsplit(dato,','));
num=size(vet,2);
rad=sqrt(num-1);
mat=reshape(vet(1:num-1), rad, rad);
[x, y] = meshgrid(1:rad, 1:rad);
mat3d=mat';
heatmap(mat3d)
caxis([100, 180]);

pause(0.1);

drawnow

end

rosshutdown;