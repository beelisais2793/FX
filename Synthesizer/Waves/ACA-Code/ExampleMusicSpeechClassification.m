function ExampleMusicSpeechClassification(cDatasetPath)

    if (nargin<1)
        % this script is written for the GTZAN music/speech dataset
        % modify this path or use the function parameter to specify your
        % dataset path
        cDatasetPath = 'd:\dataset\music_speech\'; 
    end
    if (exist('ComputeFeature') ~= 2)
        error('Please add the ACA scripts (https://github.com/alexanderlerch/ACA-Code) to your path!');
    end
    if ((exist([cDatasetPath 'music']) ~= 7) || (exist([cDatasetPath 'speech']) ~= 7))
        error('Dataset path wrong or does not contain music/speech folders!')
    end
    
    iNumFeatures = 2;
    
    % read directory contents
    music_files = dir([cDatasetPath 'music/*.au']);
    speech_files = dir([cDatasetPath 'speech/*.au']);
 
    v_music = zeros(iNumFeatures,size(music_files,1));
    v_speech = zeros(iNumFeatures,size(speech_files,1)); 
    
    % extract features, this may take a while...
    for i = 1:size(music_files, 1)
        v_music(:, i) = ExtractFeaturesFromFile_I(...
            [cDatasetPath 'music/' music_files(i).name]);
    end
    for i = 1:size(speech_files, 1)
        v_speech(:, i) = ExtractFeaturesFromFile_I(...
            [cDatasetPath 'speech/' speech_files(i).name]);
    end
    
    % assign class labels for training and eval
    C = [zeros(1, size(music_files, 1)) ones(1, size(speech_files, 1))];

    % normalize features
    v = [v_music, v_speech];
    m = mean(v, 2);
    s = std(v, 0, 2);
    v = (v - repmat(m, 1, size(music_files, 1) + size(speech_files, 1)))./...
        repmat(s, 1, size(music_files, 1)+size(speech_files, 1));
  
    % compute the overall accuracy with cross validation
    [acc, mat] = ToolLooCrossVal(v, C);
    
    disp('confusion matrix:'),
    disp(mat);

    disp('micro accuracy:'), 
    disp(sum(diag(mat)) / sum(sum(mat)))
    tmp = zeros(size(mat, 1), 1);
    for i = 1:size(mat, 1)
        tmp(i) = mat(i, i) / sum(mat(i, :));
    end
    disp('macro accuracy:'), 
    disp(mean(tmp))
    
    % compute the individual feature performance
    [acc1, mat1] = ToolLooCrossVal(v(1, :), C);
    sprintf('centroid accuracy: %f', acc1)
    [acc2, mat2] = ToolLooCrossVal(v(2 ,:), C);
    sprintf('rms accuracy: %f', acc2)
end

function [v] = ExtractFeaturesFromFile_I(cFilePath)

    cFeatureNames = char('SpectralCentroid',...
    'TimeRms');

    % read audio
    [x, fs] = audioread(cFilePath);
    x = x / max(abs(x));

    % compute first feature
    feature = ComputeFeature (deblank(cFeatureNames(1, :)), x, fs);
    v(1, 1)  = mean(feature);
    
    % compute second feature
    feature = ComputeFeature (deblank(cFeatureNames(2, :)), x, fs);
    v(2, 1)  = std(feature(1,:));
end
