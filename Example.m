%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%   This file is part of SemanticFastForward_JVCI.
%
%    SemanticFastForward_JVCI is free software: you can redistribute it and/or modify
%    it under the terms of the GNU General Public License as published by
%    the Free Software Foundation, either version 3 of the License, or
%    (at your option) any later version.
%
%    SemanticFastForward_JVCI is distributed in the hope that it will be useful,
%    but WITHOUT ANY WARRANTY; without even the implied warranty of
%    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%    GNU General Public License for more details.
%
%    You should have received a copy of the GNU General Public License
%    along with SemanticFastForward_JVCI.  If not, see <http://www.gnu.org/licenses/>.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% 1- Download our example video (in Linux bash) and move it to the project folder

$ wget www.verlab.dcc.ufmg.br/semantic-hyperlapse/data/video-example/example.mp4


% 2- Extract the optical flow information (in Windows CMD). The output file name must be the same name of the input video using the extesion ".csv".

% Vid2OpticalFlowCSV.exe -v < video_file > -c < config_file > -o < output_file >

$ ./Vid2OpticalFlowCSV.exe -v example.mp4 -c default-config.xml -o example.csv


% 3- Extract semantic information from video with "SemanticScripts/ExtractAndSave.m". Output file will be placed on the input video folder, with video file name, followed by the semantic extractor and the suffix "extracted.mat". Example: "example_face_extracted.mat".

% On MATLAB console, go to the project folder and run the following commands:

% ExtractAndSave(< video_file_path >, < semantic_extractor >);

>> addpath('SemanticScripts');
>> ExtractAndSave('example.mp4', 'face');


% Results for steps 2 (example.csv) and 3 (example_face_extracted.mat) for this example video are available for download using the link:

$ wget www.verlab.dcc.ufmg.br/semantic-hyperlapse/data/video-example/example.csv

$ wget www.verlab.dcc.ufmg.br/semantic-hyperlapse/data/video-example/example_face_extracted.mat


% 4- To generate the final hyperlapse video, use the "SpeedupVideo" function. The graph weight tuple are optimized via the PSO algorithm.

%SpeedupVideo(< video_dir >, < experiment_name >, < semantic_extractor >, ['Speedup', < speedup_rate >,] ['ExportOutputVideo', < bool_value>]);
SpeedupVideo('.', 'Example', 'face', 'Speedup', 10);

% In this script, the example video is inside the project folder, so set the < video_dir > parameter as ''.

% The user may set the optional argument 'ExportOutputVideo' as false to avoid generate the output video during the search.

%SpeedupVideo(< video_dir >, < experiment_name >, < semantic_extractor >, ['Speedup', < speedup_rate >,] ['ExportOutputVideo', < bool_value>]);
SpeedupVideo('.', 'Example', 'face', 'Speedup', 10, 'ExportOutputVideo', false);