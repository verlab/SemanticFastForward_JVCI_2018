# Project #

This project is based on the paper [Making a long story short: A multi-importance fast-forwarding egocentric videos with the emphasis on relevant objects](https://www.verlab.dcc.ufmg.br/semantic-hyperlapse/jvci2018/) on the **Special Issue on Egocentric Vision and Lifelogging Tools** at **Journal of Visual Communication and Image Representation** (JVCI 2018). It implements a fast-forward method for first-person videos based on multi-importance approach.

For more information and visual results, please acess the [project page](http://www.verlab.dcc.ufmg.br/fast-forward-video-based-on-semantic-extraction).

## Contact ##

### Authors ###

* Michel Melo da Silva - PhD student - UFMG - michelms@dcc.ufmg.com
* Washington Luis de Souza Ramos - PhD student - UFMG - washington.ramos@dcc.ufmg.br
* Felipe Cadar Chamone - Undergraduate Student - UFMG - cadar@dcc.ufmg.br
* João Pedro Klock Ferreira - Undergraduate Student - UFMG - jpklock@ufmg.br
* Mario Fernando Montenegro Campos - Advisor - UFMG - mario@dcc.ufmg.br
* Erickson Rangel do Nascimento - Advisor - UFMG - erickson@dcc.ufmg.br

### Institution ###

Federal University of Minas Gerais (UFMG)  
Computer Science Department  
Belo Horizonte - Minas Gerais -Brazil 

### Laboratory ###

![VeRLab](https://www.dcc.ufmg.br/dcc/sites/default/files/public/verlab-logo.png)  

__VeRLab:__ Vison and Robotic Laboratory  
http://www.verlab.dcc.ufmg.br

## Code ##

### Dependencies ###

* MATALB 2015a

### Usage ###

 * The following flowchart depicts the processing steps of our method:

![Flowchart](https://user-images.githubusercontent.com/23279754/28940021-a0b72954-7869-11e7-8f90-c57ee22ad9ec.jpg)

1. **Optical Flow Estimator:**

    The first step processing is to estimate the Optical Flow of the Input VIdeo. 

    1. First you should download the [Poleg et al. 2014](http://www.cs.huji.ac.il/~peleg/papers/cvpr14-egoseg.pdf) Flow Estimator code from the [link](http://www.vision.huji.ac.il/egoseg/EgoSeg1.2.zip).
    2. Navigate to the download folder and unzip the code.
    3. Into the Vid2OpticalFlowCSV\Example folder, run the command:

```bash
Vid2OpticalFlowCSV.exe -v < video_filename > -c < config.xml > -o < output_filename.csv >
```

| Options | Description | Type | Example | 			
|--------:|-------------|------|--------|
| ` < Video_filename > ` | Path and filename of the video. | _String_ | `~/Data/MyVideos/myVideo.mp4` |
| ` < config.xml > ` | Path to the configuration XML file. | _String_ | `../default-config.xml` |
| ` < output_filename.csv > ` | Path to save the output CSV file. | _String_ | `myVideo.csv` |

Save the output file using the same name of the using with extension `.csv`.

2. **Semantic Extractor:**

    The second step is to extract the semantic information over all frames of the Input video and save it in a CSV file. On MATLAB console, go to the project folder and run the command:

```matlab
>> ExtractAndSave(< Video_filename >, < Semantic_extractor_name >)
```

| Options | Description | Type | Example | 			
|--------:|-------------|------|--------|
| ` < Video_filename > ` | Path and filename of the video. | _String_ | `~/Data/MyVideos/Video.mp4` |
| ` < Semantic_extractor_name > ` | Semantic extractor algorithm. | _String_ | `'face'` or `'pedestrian'` or `'coolnet'`|

3. **Create Experiment** 

    To run the code, you should create an experiment entry. On a text editor, add a case to the `GetVideoDetails` function in the file `SemanticSequenceLibrary.m`:

```matlab
function [videoFile, startInd, endInd, filename, fps] = GetVideoDetails(video_dir,exp_name)

 ...

 case < Experiment_name >
     filename = < video_filename >;
     startInd = < start_index_frame > ;
     endInd   = < final_index_frame >;
     fps      = < video_frames_per_second >;
					
  ... 
  
```

| Field | Description | Type | Example | 			
|--------:|-------------|------|--------|
| ` < Experiment_name > ` | Name to identify the experiment. | _String_ | `MyVideo` |
| ` < video_filename > ` | Filename to the video. | _String_ | `myVideo.mp4` |
| ` < start_index_frame > ` | Frame index to start the processing. | _Integer_ | `1` |
| ` < final_index_frame > ` | Frame intex to stop the processing. | _Integer_ | `16987` |
| ` < video_frames_per_second > ` | Frames per second of the video. | _Integer_ | `30` |

4. **Semantic Fast-Forward**

    After the previous steps, you are ready to accelerate the Input Video. On MATLAB console, go to the project folder and run the command:

```matlab
>> SpeedupVideo(< Video_dir >, < Experiment_name >, < Semantic_extractor_name >, ['Speedup', < rate >] )
```			

| Options | Description | Type | Example | 			
|--------:|-------------|------|--------|
| ` < Video_dir > ` | Path to the folder where the video file is. | _String_ | `~/Data/MyVideos` |
| ` < Experiment_name > ` | Name set in the SemanticSequenceLibrary.m file. | _String_ | `My_Video` |
| ` < Semantic_extractor_name > ` | Semantic extractor algorithm used in the Semantic Extractor step. | _String_ | `'face'` or `'pedestrian'` or `'coolnet'`|
| ` < rate > ` | Fast-forward rate _[default value is 10]_. | _Integer_ | `8` |   

5. **Configure Video Parameters**

    After the Semantic Fast-Forward step, the accelerated video is create. Now we are going to stabilize the output video. The first stabilization step is to configure the video parameters in the file `acceleratedVideoStabilizer/experiment.xml`. Follow the instructions described into the file.

6. **Accelerate Video Stabilizer**

    Navigate to the `<project_folder>/acceleratedVideoStabilizer/` folder. Follow the instructions described into the `<project_folder>/acceleratedVideoStabilizer/README.md` file to compile and run the code.
    
	The output of this step is the stabilized semantic fast-forward video. 

## Acknowledgements ##

1.	__EgoSampling__: Y. Poleg, T. Halperin, C. Arora, S. Peleg, Egosampling: Fast-forward and stereo for egocentric videos, in: IEEE Conference on Computer Vision and Pattern Recognition, 2015, pp. 4768–4776. doi:10.1109/CVPR.2015.7299109.
2.  __LK_Blocked_Optical_Flow__: Y. Poleg, C. Arora, S. Peleg, Temporal segmentation of egocentric videos, in: IEEE Conference on Computer Vision and Pattern Recognition, 2014, pp. 2537–2544. doi:10.1109/CVPR.2014.325.
3.  __NPD_Face_Detector__: S. Liao, A. K. Jain, S. Z. Li, A fast and accurate unconstrained face detector, IEEE Transactions on Pattern Analysis and Machine Intelligence 38 (2) (2016) 211–223. doi:10.1109/TPAMI.2015.2448075.
4.  __PMT_Pedestrian_Detector__: P. Dollar, Piotr’s Computer Vision Matlab Toolbox (PMT), https://github.com/pdollar/toolbox.

## Citation ##

If you are using it to academic purpose, please cite: 

M. M. Silva, W. L. S. Ramos, F.C. Chamone, J. P. K. Ferreira, M. F. M. Campos, E. R. Nascimento, __Making a long story short: A multi-importance fast-forwarding egocentric videos with the emphasis on relevant objects__, in: Special Issue on Egocentric Vision and Lifelogging Tools at Journal of Visual Communication and Image Representation. doi:10.1016/j.jvcir.2018.02.013.

### Bibtex entry ###

> @article{Silva2018,
> title = {Making a long story short: A Multi-Importance fast-forwarding egocentric videos with the emphasis on relevant objects},  
> author = {Michel M. Silva and Washington L. S. Ramos and Felipe C. Chamone and João P. K. Ferreira and Mario F. M. Campos and Erickson R. Nascimento},  
> journal = {Journal of Visual Communication and Image Representation},  
> volume = {53},  
> number = {},  
> pages = {55 – 64},  
> year = {2018},  
> issn = {1047-3203},  
> doi = {10.1016/j.jvcir.2018.02.013}  
> }

#### Enjoy it. ####
