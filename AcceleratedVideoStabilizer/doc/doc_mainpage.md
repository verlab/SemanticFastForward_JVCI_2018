\mainpage General Information

\section project_sec Project

This code is based on the paper [Towards Semantic Fast-Forward and Stabilized Egocentric Video](http://www.verlab.dcc.ufmg.br/wp-content/uploads/2016/10/Final_Draft_ECCVW_2016_Towards_Semantic_Fast_Forward_and_Stabilied_Egocentric_Videos.pdf) on the <b>First International Workshop on Egocentric Perception, Interaction and Computing</b> at <b>European Conference on Computer Vision</b> (EPIC@@ECCV 2016). The goal of the program is to stabilize a fast-forward version of a video, using the dropped frames to reconstruct distorted images during the processing. 

For more information, acess the link: http://www.verlab.dcc.ufmg.br/fast-forward-video-based-on-semantic-extraction/. \n

\section contact_sec Contact

\subsection authors_sec Authors:

\li Michel Melo da Silva - PhD student - UFMG - michelms@dcc.ufmg.com
\li Washington Luis de Souza Ramos - MSc student - UFMG - washington.ramos@outlook.com
\li João Pedro Klock Ferreira - Undergraduate Student - UFMG - jpklock@dcc.ufmg.br
\li Mario Fernando Montenegro Campos - Advisor - UFMG - mario@dcc.ufmg.br
\li Erickson Rangel do Nascimento - Advisor - UFMG - erickson@dcc.ufmg.br

\subsection institution_sec Institution

Federal University of Minas Gerais (UFMG) \n
Computer Science Department \n
Belo Horizonte - Minas Gerais -Brazil 

\subsection laboratory_subsec Laboratory

\b VeRLab: Vison and Robotic Laboratory \n
http://www.verlab.dcc.ufmg.br

\section program_sec Program

\subsection dependencies_sec Dependencies

\li OpenCV 2.4.X <i>(Tested with 2.4.9 and 2.4.13)</i>
\li Armadillo 6.X <i>(Tested with 6.600.5 -- Catabolic Amalgamator)</i>
\li Boost 1.X <i>(Tested with 1.54.0)</i>

\subsection compiling_sec Compiling

The program can be compiled using either \c cmake or \c qmake tools.
Into the project directory, run the following commands: 

<tt>
<b>user@@computer:@<project_path>:</b> mkdir build  \n
<b>user@@computer:@<project_path>:</b> cd build \n
<b>user@@computer:@<project_path>:</b> cmake .. \n
<b>user@@computer:@<project_path>:</b> make \n
<b>user@@computer:@<project_path>:</b> ./VideoStabilization < Settings_file > [-h] [Range_min] [Range_max]\n
</tt>

\subsection execution_sec Exemples of usage

<tt> < Program_name > < Settings_file > [-h] [Range_min = 0 ] [Range_max = num_frames ] </tt> \n\n
Example 1: Run VideoStabilization using Help option. \n
<tt> -> VideoStabilization -h </tt> \n\n
Example 2: Run VideoStabilization in the Experiment_1 processing the whole video. \n
<tt> -> VideoStabilization Experiment_1.xml </tt> \n\n
Example 3: Run VideoStabilization in the Experiment_1 processing from the 150 frame until the last one. \n
<tt> -> VideoStabilization Experiment_1.xml \c 150 </tt> \n\n
Example 4: Run VideoStabilization in the Experiment_1 processing from the 150 frame until the frame 490. \n
<tt> -> VideoStabilization Experiment_1.xml 150 490 </tt> \n\n
 
\section citation_sec Citation

If you are using it to academic purpose, please cite: \n

M. M. Silva, W. L. S. Ramos, J. P. K. Ferreira, M. F. M. Campos, E. R. Nascimento, <b>Towards semantic fast-forward and stabilized egocentric videos</b>, in: European Conference on Computer Vision Workshops, Springer International Publishing, Amsterdam, NL, 2016, pp. 557–571. doi:10.1007/978-3-319-46604-0_40.

\subsection bibtex_subsec Bibtex entry

<tt>
@@InBook{Silva2016, \n
  Title     = {Towards Semantic Fast-Forward and Stabilized Egocentric Videos}, \n
  Author    = {Silva, Michel Melo and Ramos, Washington Luis Souza and Ferreira,  \n
              Joao Pedro Klock and Campos, Mario Fernando Montenegro and Nascimento, Erickson Rangel}, \n
  Editor    = {Hua, Gang and J{\'e}gou, Herv{\'e}}, \n
  Pages     = {557--571}, \n
  Publisher = {Springer International Publishing}, \n
  Year      = {2016}, \n
  Address   = {Cham}, \n
  Booktitle = {Computer Vision -- ECCV 2016 Workshops: Amsterdam, The Netherlands,  \n
              October 8-10 and 15-16, 2016, Proceedings, Part I}, \n
  Doi       = {10.1007/978-3-319-46604-0_40}, \n
  ISBN      = {978-3-319-46604-0}, \n
  Url       = {http://dx.doi.org/10.1007/978-3-319-46604-0_40} \n
}
</tt>