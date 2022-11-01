%period vs initial velocity
clear all
clf

%filename = 'finalq1a_rk4_vinitial_negative100_v1';
%filename = 'finalq1a_rk4_vinitial_1_gamma_neg3_v1';
%filename = 'finalq1a_rk4_vinitial_0pt1_gamma_neg0pt1_v1';
%filename = 'finalq1a_test3'
%filename = 'finalq1a_rk4_vinitial_1_v1'
%filename = 'finalq1a_period_gamma'
%filename = 'finalq1b_damp_2.94000_vi_';

%str = num2str(A,formatSpec)

driveFreq = 1.0
driveFreqArray = 1:5

for outerindex = 1:numel(driveFreqArray)
    figure
driveFreq = driveFreqArray(outerindex)



%%%%%%%%%%%%
dampingArray = 1:5
for index = 1:numel(dampingArray)
   legendtext{index} =  sprintf('f = %2.5f',dampingArray(index));
end

countoutside = 1;
for dampConstant = dampingArray

    
%figure
driveFreq = num2str(driveFreq,'%2.5f')
driveAmp = num2str(dampConstant,'%2.5f')
%filestring = sprintf('finalq1c_driveAmp_%s_vi_2', str)
filestring = sprintf('finalq1c_driveAmp_%s_driveFreq_%s', driveAmp, driveFreq)
filename = filestring

filenamewithextension = strcat(filename, '.dat');
datacomplete = importdata(filenamewithextension);
dataset = datacomplete.data
datatext = datacomplete.textdata
textsecond = datatext(2)
textsecond = textsecond{1}
damptext = textsecond
damptext2 = damptext(2:numel(damptext))
dampingConstant = str2double(damptext2)




stepsize = dataset(:,1);
time = dataset(:,2);
position = dataset(:,3);
velocity = dataset(:,4);
energy_residual = dataset(:,5);
gamma = dataset(:,6);
velocity_initial = dataset(1,4);


cc=hsv(12);
hold on;
%plot(time,position,'color',cc(randi(11),:));

%plot(time,position, '-*','color',cc(randi(11),:));
%plot(position(1),velocity(1), 'o');


%clf
%plot(position, velocity, '-*','color',cc(randi(11),:));
plot(position, velocity, '*','color',cc(countoutside,:));
%plot(time, position, '-*','color',cc(countoutside,:));
textdragon = sprintf('velocity vs position, + gamma, f = %2.5f, omega = %s',dampConstant, driveFreq);
title(textdragon);
ylabel('velocity')
xlabel('position')
legend(legendtext);

signarray = sign(position);
signarray(signarray<0)=0
alltrue(countoutside) = all(signarray); %if all true then critically damped

countoutside = countoutside + 1;
end


%%%%%%%%%%%%%%%
SetPaper(gcf,8,6) %x,y
DoPlotPDF(filename)
end

SetPaper(gcf,8,6) %x,y
DoPlotPDF('velocity_position_poincare_negative_gamma2')


