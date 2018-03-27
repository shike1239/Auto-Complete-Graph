#ifndef AUTOCOMPLETEGRAPH_ACGMCLLOCALIZATION_26072018
#define AUTOCOMPLETEGRAPH_ACGMCLLOCALIZATION_26072018

#include <pcl/common/transforms.h>
#include <ndt_mcl/particle_filter.hpp>

#include "auto_complete_graph/Localization/Localization.hpp"
#include "auto_complete_graph/GraphMapLocalizationMsg.h"
#include "auto_complete_graph/Localization/LocalizationConvertion.hpp"

namespace AASS {

namespace acg{

	/**
	 * A class doing the localization published in graph_map_publisher_localization.cpp
	 */
	class ACGMCLLocalization : public perception_oru::particle_filter{

	protected:
		
// 		boost::shared_ptr<perception_oru::particle_filter> _ndtmcl;
		bool mcl_loaded_;
		
		std::vector<Localization> _localization;


	public:
		// map publishing function
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW


		ACGMCLLocalization(std::string mapFile_, int particleCount_) : mcl_loaded_(false), perception_oru::particle_filter(mapFile_, particleCount_){};
		
		ACGMCLLocalization(perception_oru::NDTMap *ndtMap_, int particleCount_/*, init_type initializationType_*/, bool be2D_=true, bool forceSIR_=true,double varLimit_=0, int sirCount_=0) : mcl_loaded_(false), perception_oru::particle_filter(ndtMap_, particleCount_, be2D_, forceSIR_, varLimit_, sirCount_){}
		
		void init(double xx, double yy, double yaw, double initVar, double cov_x_mcl, double cov_y_mcl, double cov_yaw_mcl, double scale_gaussian_mcl, double numPart, bool forceSIR){

			std::cout << "INIT MCL ACG" << std::endl;
			
// 			perception_oru::particle_filter* pMCL = new perception_oru::particle_filter(map, numPart, true, forceSIR);
// 			_ndtmcl = boost::shared_ptr<perception_oru::particle_filter>(pMCL);
			
			setMotionModelCovX(cov_x_mcl);
			setMotionModelCovY(cov_y_mcl);
			setMotionModelCovYaw(cov_yaw_mcl);
			setScalingFactorGaussian(scale_gaussian_mcl);
			
	 		std::cout << "Init at " << xx << " " << yy << " " << yaw << std::endl;
			InitializeNormal(xx, yy, yaw, initVar);
			std::cout << "ormal init" << std::endl;
			mcl_loaded_ = true;
			std::cout << "MAP LOADED SUCCESSFULLY :)" << std::endl;
			
		}
		
		
		///Laser scan to PointCloud expressed in the base frame
		std::tuple<Eigen::Vector3d, Eigen::Matrix3d> localization( const Eigen::Affine3d& Tmotion, const pcl::PointCloud<pcl::PointXYZ>& cloud, const Eigen::Affine3d& sensorpose, double fraction, double cutoff){
			
			if(mcl_loaded_ = true){
				
				UpdateAndPredictEff(Tmotion, cloud, sensorpose, fraction, cutoff);

				Eigen::Matrix3d cov;
				Eigen::Vector3d mean;

				GetPoseMeanAndVariance2D(mean, cov);
				
				return std::make_tuple(mean, cov);
			// 	
			}
			else{
				std::cout << "You need to init MCL to start MCL localization" << std::endl;
			}
		}
		
		
		void savePos(int index = -1){
			Eigen::Matrix3d cov;
			Eigen::Vector3d mean;
			GetPoseMeanAndVariance2D(mean, cov);
			Localization loc;
			loc.mean = mean;
			loc.cov = cov;
			loc.index = index;
			_localization.push_back(loc);

//			_mean_saved.push_back(mean);
//			_cov_saved.push_back(cov);
//			_indexes.push_back(index);
			
		}
		
//		const std::vector<Eigen::Vector3d>& getMeans() const {return _mean_saved;}
//		const std::vector<Eigen::Matrix3d>& getCovs() const {return _cov_saved;}

		const std::vector<Localization>& getLocalizations() const {return _localization;}

		
		void toMessage(auto_complete_graph::GraphMapLocalizationMsg& msg){

			for(auto const& localization : _localization) {
				auto_complete_graph::LocalizationMsg loc_msg = AASS::acg::toMessage(localization);
				msg.localizations.push_back(loc_msg);
			}
			

		}
		
	};
	
	
	

}

}

#endif