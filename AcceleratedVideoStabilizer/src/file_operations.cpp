//////////////////////////////////////////////////////////////////////////////////////
////   This file is part of SemanticFastForward_EPIC@ECCVW.
//
//    SemanticFastForward_EPIC@ECCVW is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    SemanticFastForward_JVCI is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with SemanticFastForward_EPIC@ECCVW.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////////////

/**
 * @file file_operations.cpp
 *
 * Functions to manipulate files.
 *
 * Functions to read from TXT and CSV files, write data on files.
 */

#include "headers/file_operations.h"

/**
 * @brief Function to load the master frames from a file defined in the field read_masterframes_filename in the experiment_settings.
 *
 * @param experiment_settings - variable with the experiment setting.
 *
 * @return \c std::vector - vector of master frames loaded from file describle in the experiment settings.
 *
 * @author Michel Melo da Silva
 * @date 08/04/2016
 */
std::vector<int> loadMasterFramesFromFile (const EXPERIMENT &experiment_settings ) {

    std::ifstream file (experiment_settings.read_master_frames_filename.c_str());

    if ( file.is_open() ) {

        int numberOfMasters;
        file >> numberOfMasters;

        std::vector<int> masters ( numberOfMasters );

        for ( int i = 0; i < numberOfMasters ; i ++ )
            file >> masters[i];

        file.close();

        return masters;

    } else {
        std::cout << " --(!) ERROR: Can not open the file \"" << experiment_settings.read_master_frames_filename << "\"." << std::endl;
        exit(-3);
    }
}

/**
 * @brief Function to save the calculated master frames into a file defined in the field save_masterframes_filename in the experiment_settings.
 *
 * @param experiment_settings - variable with the experiment setting.
 * @param masters_frames - vector of master frames to be saved in file describle in the experiment settings.
 *
 * @return
 *      \c bool \b true  - if the writer return sucess while writing the file. \n
 *      \c bool \b false - if the writer return fail while writing the file.
 *
 * @author Michel Melo da Silva
 * @date 08/04/2016
 */
bool saveMasterFramesToFile (const EXPERIMENT &experiment_settings , const std::vector<int> masters_frames ) {

    std::ofstream file (experiment_settings.save_master_frames_filename.c_str());

    if ( file.is_open() ) {

        file << masters_frames.size() << std::endl ;

        for ( unsigned int i = 0; i < masters_frames.size() ; i ++ )
            file << masters_frames[i] << std::endl ;

        file.close();

        return true;

    } else {
        std::cout << " --(!) ERROR: Can not open the file \"" << experiment_settings.save_master_frames_filename << "\"." << std::endl;
        exit(-3);
    }

    return false;
}


/**
 * @brief Function to read a CSV composed by frames selected by the selection algorithm.
 *
 * @param filename - std::string with the filename of the CSV file that contains the selected frames of the reduced video.
 * @param selected_frames - vector to save the selected frames.
 *
 * @return \c void
 *
 * @author Washington Luis de Souza Ramos
 * @date 14/04/2016
 */
void readSelectedFramesCSV(std::string filename, std::vector<int>& selected_frames)
{
    std::ifstream file(filename.c_str(), std::ios::in );
    if (!file.is_open()){
        std::cerr << " --(!) ERROR: Can not open the file \"" << filename << "\"." << std::endl;
        exit(-6);
    }

    std::string line;

    while (std::getline(file,line))
    {
        std::stringstream lineStream(line);
        std::string cell;

        std::getline(lineStream,cell,',');
        selected_frames.push_back(atoi(cell.c_str()));
    }

    file.close();
}

/**
 * @brief Function that manages the experiments ID automatically.
 *
 * @return \c std::string - The experiment id formatted by 4 digits
 *
 * @author Washington Luis de Souza Ramos
 * @date 19/04/2016
 */
std::string getExperimentId(){
    std::string line;
    std::ifstream  id_manager_file_in;
    std::ofstream  id_manager_file_out;
    int experiment_id;

    id_manager_file_in.open("ID_MANAGER");

    //The file does not exist
    if (!id_manager_file_in.is_open()){
        id_manager_file_out.open("ID_MANAGER");
        id_manager_file_out << "1";
        id_manager_file_out.close();
        id_manager_file_in.open("ID_MANAGER");
    }

    //Reading + Writing
    id_manager_file_out.open("ID_MANAGER_AUX");
    id_manager_file_in >> line;
    experiment_id = atoi(line.c_str());
    id_manager_file_out << experiment_id + 1;

    //Closing files
    id_manager_file_in.close();
    id_manager_file_out.close();

    //Renaming
    remove("ID_MANAGER");
    rename("ID_MANAGER_AUX", "ID_MANAGER");

    std::stringstream ss;
    ss << std::setw(4) << std::setfill('0') << experiment_id;

    return ss.str();
}

/**
 * @brief Function that set the file pointer to the desired line, if it is smaller than the file number of lines.
 *
 * @param file - std::ifstream object where the file is read.
 * @param line - line where the header read will be set.
 *
 * @return \c std::istream - Reference to the file in the selected line.
 *
 * @author Michel Melo da Silva
 * @date 30/04/2016
 */
std::istream& goToLine(std::istream& file, const unsigned int line){
    file.seekg(std::ios::beg);
    for ( unsigned int i = 0 ; i < line ; ++i ){
        file.ignore( std::numeric_limits<std::streamsize>::max() , '\n' );
    }
    return file;
}

/**
 * @brief Function that returns the semantic cost of the transition from the frame_src to
 *          the frame_dst. The file where the semantic cost will be find is defined in the experiment_settings
 *
 * @param experiment_settings - variable with the experiment setting.
 * @param frame_src - first frame of the transition.
 * @param frame_dst - last frame of the transition.
 *
 * @return \c double - The semantic cost of the transiction of the frame_src to the frame_dst. Returns the complementary value ( 1 - x ).
 *
 * @author Michel Melo da Silva
 * @date 30/04/2016
 */
double getSemanticCost ( const EXPERIMENT &experiment_settings , const int frame_src , const int frame_dst ){

    std::ifstream file ( experiment_settings.semantic_costs_filename.c_str(), std::ios::in );

    if ( !file.is_open() ){
        std::cerr << " --(!) ERROR: Can not open the CSV file \"" << experiment_settings.semantic_costs_filename << "\" with the semantic costs." << std::endl;
        exit(-10);
    }

    //Read the first two lines to find the frame ranges for the video
    std::string line, cost;

    getline(file,line);
    std::istringstream drift_string( line );
    getline( drift_string, cost, ',' );

    double shift = std::atof(cost.c_str());

    int line_number = (frame_src-shift+1)+1;

    goToLine(file, line_number);

    getline(file,line);

    file.close();

    std::istringstream ss( line );

    for ( int i = 0 ; i < (frame_dst - frame_src) ; i ++ ) {
        if (!getline( ss, cost, ',' )) {
            std::cerr << " --(!) ERROR: Transition from frame_src to frame_dst larger than number of transitions describle int the file the semantic costs." << std::endl;
            exit(-11);
        }
    }

    return ( 1.0f - std::atof(cost.c_str()) ) ;

}

//double getJitterCost(const EXPERIMENT &experiment_settings , const int frame_src , const int frame_dst){
//}

/**
 * @brief loadInstabilityCostsFromFile Loads the instability costs from a CSV file (matrix format).
 * @param experiment_settings
 * @return The matrix of instability costs
 *
 * @author Washington Luis de Souza Ramos
 * @date 12/09/2016
 */
std::vector< std::vector<double> > loadInstabilityCostsFromFile(const EXPERIMENT &experiment_settings){

    std::ifstream file ( experiment_settings.instability_costs_filename.c_str(), std::ios::in );

    if ( !file.is_open() ){
        std::cerr << " --(!) ERROR: Can not open the CSV file \"" << experiment_settings.instability_costs_filename << "\" with the instability costs." << std::endl;
        exit(-10);
    }

    std::vector< std::vector<double> > instability_costs;

    std::string line;
    while(std::getline(file,line))
    {
        std::stringstream  lineStream(line);
        std::string        cell;
        std::vector<double> current_instability_costs;
        while(std::getline(lineStream,cell,','))
        {
            current_instability_costs.push_back(atof(cell.c_str()));
        }
        instability_costs.push_back(current_instability_costs);
    }

    return instability_costs;
}

/**
 * @brief loadOpticalFlow Loads the Optical Flow calculated by the FlowNet from a CSV file.
 * @param experiment_settings
 * @return Vector[j][i] with the optical flow from the image j to j+1 (i=0 is dx and i=1 is dy).
 *
 * @author Michel Melo da Silva
 * @date 09/01/2017
 */
std::vector< std::vector<double> > loadOpticalFlow(const EXPERIMENT &experiment_settings){

    std::ifstream file ( experiment_settings.optical_flow_filename.c_str(), std::ios::in );

    if ( !file.is_open() ){
        std::cerr << " --(!) ERROR: Can not open the CSV file \"" << experiment_settings.optical_flow_filename << "\" with the optical flow." << std::endl;
        exit(-12);
    }

    std::vector< std::vector<double> > optical_flow;

    std::string line;

    while(std::getline(file,line))
    {
        std::stringstream  lineStream(line);
        std::string        cell;
        std::vector<double> current_optical_flow;

        while(std::getline(lineStream,cell,','))
            current_optical_flow.push_back(atof(cell.c_str()));

        std::vector<double> flows (2);
        flows[0] = current_optical_flow[3];
        flows[1] = current_optical_flow[4];

        optical_flow.push_back(flows);
    }

    return optical_flow;
}

/**
 * @brief str2bool convert a string to boolean.
 * @param string
 * @return boolean
 *
 * @author Felipe Cadar Chamone
 * @date 14/07/2017
 */
bool str2bool(std::string str){
  std::size_t found = str.find("true");
  if (found!=std::string::npos){
    return true;
  }
  else{
    return false;
  }
}

/**
 * @brief filter_string filter a string to remove white spaces and line break.
 * @param string
 * @return string
 *
 * @author Felipe Cadar Chamone
 * @date 14/07/2017
 */
std::string filter_string(std::string str){
  boost::erase_all(str, "\n");
  boost::erase_all(str, " ");
  return str;
}
