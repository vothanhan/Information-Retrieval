clear all; close all;
%% init parameter
addpath('AKM');
run('vlfeat\toolbox\vl_setup.m');
datasetDir = 'oxford\images\';
num_words = 20000;
num_iterations = 7;
num_trees = 12;
dim = 128;
if_weight = 'tfidf';
if_norm = 'l1';
if_dist = 'l1';
verbose=1;
%% Compute SIFT features
if ~exist('oxford\feat\feature.bin', 'file')    %if already calculated, only load
    fprintf('Computing SIFT features:\n');%else
    
    features = zeros(128, 2000000); %initiate feature matrix
    nfeat = 0;  %initiate number of feature
    files = dir(fullfile(datasetDir, '*.jpg')); %set of *.jpg file in data
    nfiles = length(files); %number of image
    features_per_image = zeros(1,nfiles);   %initiate feature number matrix
    for i=1:nfiles%loop through all image
        fprintf('Extracting features %d/%d\n', i, nfiles);
        imgPath = strcat(datasetDir, files(i).name);    %image path
        I = im2single(rgb2gray(imread(imgPath)));   %convert grayscale image to single 
        I = imresize(I, 0.6);   %reduce size
        [frame, sift] = vl_covdet(I, 'method', 'Hessian', 'estimateAffineShape', true); %get sift score and frame
        %calculate rootsift
        for j = 1:size(sift,2)
            sift(:,j)=sift(:,j)/norm(sift(:,j),1);    %L1 normalize
        end
        sift=sqrt(sift);    %get square root
        %
        if nfeat+size(sift,2) > size(features,2)    %if there are more feature than initiate created(200000)
            features = [features zeros(128,1000000)];   %extend feature
        end
        features(:,nfeat+1:nfeat+size(sift,2)) = sift;%add the new feature vector
        nfeat = nfeat+size(sift,2); %increase number of feature accordingly
        features_per_image(i) = size(sift, 2);  %save number of feature per image
    end
    features = features(:,1:nfeat); %
    fid = fopen('oxford\feat\feature.bin', 'w');%open feature.bin(create if not available)
    fwrite(fid, features, 'float');%save feature 
    fclose(fid);
    
    save('oxford\feat\feat_info.mat', 'features_per_image', 'files');   %save number of feature per image
else
    fprintf('Loading SIFT features:\n');    %load feature
    file = dir('oxford\feat\feature.bin');
    %features = zeros(128, file.bytes/(4*128), 'single');

    fid = fopen('oxford\feat\feature.bin', 'r');
    features = fread(fid, [128, file.bytes/(4*128)], 'float');
    fclose(fid);
    
    load('oxford\feat\feat_info.mat');
end

%% compute rootSIFT
fprintf('Computing rootSIFT features:\n');
num_features = size(features, 2);
%rootSIFT = zeros(dim, num_features);

%for k = 1:5000000:num_features
     %eIdx = k+5000000-1;
   %if eIdx > num_features
       %eIdx = num_features;
   %end
     %for i=k:eIdx
         %features(:, i) = sqrt(features(:, i) / sum(features(:,i)));
     %end
%end

%% Run AKM to build dictionary
fprintf('Building the dictionary:\n');
num_images = length(files); 
dict_params =  {num_iterations, 'kdt', num_trees};  %specify dict parameter: iteration, number of tree,type of tree

% build the dictionary
if exist('oxford\feat\dict.mat', 'file')
    load('oxford\feat\dict.mat');
else
    randIndex = randperm(size(features,2)); 
    dict_words = ccvBowGetDict(features(:,randIndex(1:100000)), [], [], num_words, 'flat', 'akmeans', ...
        [], dict_params);%create dict by akmean, with iteration and number of tree as specified above, number of word and a number of random features
    save('oxford\feat\dict.mat', 'dict_words');
end

% compute sparse frequency vector
fprintf('Computing the words\n'); 
dict = ccvBowGetWordsInit(dict_words, 'flat', 'akmeans', [], dict_params);  %initialize quantizing word

if exist('oxford\feat\words.mat', 'file')
    load('oxford\feat\words.mat');
else
    words = cell(1, num_images);    
    for i=1:num_images%go through each image
        fprintf('Quantizing %d/%d images\n', i, num_images);
        if i==1
            bIndex = 1;
        else
            bIndex = sum(features_per_image(1:i-1))+1;%get the starting position of feature of image i
        end
        eIndex = bIndex + features_per_image(i)-1;
        words{i} = ccvBowGetWords(dict_words, features(:, bIndex:eIndex), [], dict);%computes word quantizations
    end;
    save('oxford\feat\words.mat', 'words');
end
%fprintf('Computing sparse frequency vector\n');
%dict = ccvBowGetWordsInit(dict_words, 'flat', 'akmeans', [], dict_params);
%words = ccvBowGetWords(dict_words, root_sift, [], dict);
%ccvBowGetWordsClean(dict);

% create an inverted file for the images
fprintf('Creating and searching an inverted file\n');
inv_file = ccvInvFileInsert([], words, num_words);  %create inverted index
ccvInvFileCompStats(inv_file, if_weight, if_norm);%computes stats for the input inverted file
%save('inverted_file.mat', 'if_weight', 'if_norm', 'if_dist', 'inv_file');

%% Query images
q_files = dir(fullfile('oxford\groundtruth', '*query.txt'));
%oxc1_all_souls_000013 136.5 34.1 648.5 955.7
ntop = 0;
% load query image
for k=1:length(q_files) %go through all query
    k
    fid = fopen(strcat('oxford\groundtruth\', q_files(k).name), 'r');% open file
    str = fgetl(fid);
    [image_name, remain] = strtok(str, ' ');% get image name
    fclose(fid);
    numbers = str2num(remain);  %get parameter of image
    x1 = numbers(1);
    y1 = numbers(2);
    x2 = numbers(3);
    y2 = numbers(4);
    file = strcat('oxford\images\', image_name(6:end), '.jpg');
    I = im2single(rgb2gray(imread(file)));  %convert image
    %imshow(I); hold on;
    %plot([x1 x2], [y1 y1], 'g');
    %plot([x2 x2], [y1 y2], 'g');
    %plot([x2 x1], [y2 y2], 'g');
    %plot([x1 x1], [y2 y1], 'g');
    %hold off;
    % compute rootSIFT features
    [frame, sift] = vl_covdet(I, 'method', 'Hessian', 'estimateAffineShape', true); % compute sift score
    sift = sift(:,(frame(1,:)<=x2) &  (frame(1,:) >= x1) & (frame(2,:) <= y2) & (frame(2,:) >= y1));%limit sift score to be inside limit
    for j = 1:size(sift,2)  %compute root sift
            sift(:,j)=sift(:,j)/norm(sift(:,j),1);
    end
    sift=sqrt(sift);
    % Test on an image
    q_words = cell(1,1);
    q_words{1} = ccvBowGetWords(dict_words, double(sift), [], dict);    %compute word quantization of query
    [ids, dists] = ccvInvFileSearch(inv_file, q_words(1), if_weight, if_norm, if_dist, ntop);   %find the word of query image in dict, return nearest neighbor and distance
    % visualize
    if verbose ==1
        close all;
        hold on; subplot(3,5,3); imshow(I);
        title(image_name(6:end));
    end
    fid = fopen('oxford\groundtruth\rank_list.txt', 'w');   %save rank list
    for i=1:size(ids{1},2)
        % Show only 10 highest score images
        if verbose==1 && i<=10  
            subplot(3, 5, 5+i); 
            imshow(imread(fullfile('oxford\images\', files(ids{1}(i)).name)));
            title(files(ids{1}(i)).name);
        end
        fprintf(fid, '%s\n', files(ids{1}(i)).name(1:end-4));
    end
    fclose(fid);
    script = ['oxford\groundtruth\Test.exe oxford\groundtruth\', ...
        q_files(k).name(1:end-10), ...
        ' oxford\groundtruth\rank_list.txt',...
        ' >oxford\result\', image_name(6:end), '_result.txt']; %q_files(k).name(1:end-10)
    system(script);
    if verbose==1
        %pause;
    end
end

r_files = dir(fullfile('oxford\result\', '*.txt'));
acc = [];
for i=1:length(r_files)
    file = ['oxford\result\' r_files(i).name];  
    fid = fopen(file, 'r');
    acc = [acc fscanf(fid, '%f')];
    fclose(fid);
end
mean(acc)%get average of accuracy

% clear inv file
ccvInvFileClean(inv_file);