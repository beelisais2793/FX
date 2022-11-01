clear all
clf

%filename = 'finalq1a_rk4_vinitial_negative100_v1';
%filename = 'finalq1a_rk4_vinitial_1_gamma_neg3_v1';
%filename = 'finalq1a_rk4_vinitial_0pt1_gamma_neg0pt1_v1';
%filename = 'finalq1a_test3'
%filename = 'finalq1a_rk4_vinitial_1_v1'
filename = 'finalq1a_error'

filenamewithextension = strcat(filename, '.dat');
dataset = importdata(filenamewithextension);


stepsize = dataset(:,1);
time = dataset(:,2);
position = dataset(:,3);
velocity = dataset(:,4);
energy_residual = dataset(:,5);
gamma = dataset(:,6);
velocity_initial = dataset(1,4);



%gamma change is the row number of the complete data file at which gamma
%changes
countoutside = 1;
for indexgamma = 2:numel(gamma)
    if( gamma(indexgamma) ~= gamma(indexgamma-1))
        gammachange(countoutside) = indexgamma;
        countoutside = countoutside + 1;
    end
end




hold on

%splitting up gamma into separate files
for gammaChangeCount = 1:numel(gammachange)-1
    startRow= gammachange(gammaChangeCount);
    endRow = gammachange(gammaChangeCount+1)-1;
    dataSetGamma(:,:,gammaChangeCount) = dataset(startRow:endRow,:);
end


for dataSetGammaPage = 1:size(dataSetGamma,3)
    dataSetTemp = dataSetGamma(:,:,dataSetGammaPage)
    stepsizeTemp = dataSetTemp(:,1);
    timetemp = dataSetTemp(:,2);
    positiontemp = dataSetTemp(:,3);
    velocityTemp = dataSetTemp(:,4);
    energy_residualTemp = dataSetTemp(:,5);
    gammaTemp = dataSetTemp(:,6);
    velocity_initialTemp = dataSetTemp(1,4);
    
  


%finding the period
numIntersects = 0;
for index = 1:numel(positiontemp)-1
    if positiontemp(index)*positiontemp(index+1)<0
        numIntersects = numIntersects + 1;
        timefinal = timetemp(index);
    end
end
period(dataSetGammaPage) = timefinal/(numIntersects/2);
hold on
syms x
%ezplot(sin(2*pi/T*x))
%end of finding the period
end


gammavalues = -9:9
%plot(gammavalues,period, '-*b')
%plot3(gammavalues,period,period)
%plot3(sin(t),cos(t),t)
plot(timetemp, energy_residualTemp);

textdragon = sprintf('energy residual vs time ; v0 = %d', velocity_initial);
title(textdragon);
ylabel('energy residual')
xlabel('time')
indexgamma = indexgamma + 1;






SetPaper(gcf,8,6) %x,y
DoPlotPDF('energy_residual_time')

