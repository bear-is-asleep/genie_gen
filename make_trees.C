#include "TString.h" 
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"

#include "Framework/Ntuple/NtpMCTreeHeader.h" 
#include "Framework/Ntuple/NtpMCEventRecord.h" 
#include "Framework/EventGen/EventRecord.h" 
#include "Framework/ParticleData/BaryonResUtils.h"

#include "Framework/GHEP/GHepParticle.h"       
#include "Framework/ParticleData/PDGUtils.h"
#include "Framework/GHEP/GHepStatus.h"

#include "Framework/ParticleData/PDGCodes.h"
#include <string>
#include <iostream>

using namespace genie;
using std::to_string;

void make_tree(TString filename = "gntp.0.ghep.root", TString dir=""){
  std::vector<int> nu_pdgs = {12,14,-12,-14};

  //Final muon
  std::vector<Float_t> final_mu_E;
  std::vector<Float_t> final_mu_px;
  std::vector<Float_t> final_mu_py;
  std::vector<Float_t> final_mu_pz;

  //Initial nu
  std::vector<Int_t> initialnu_pdg;
  std::vector<Float_t> initialnu_E;
  std::vector<Float_t> initialnu_px;
  std::vector<Float_t> initialnu_py;
  std::vector<Float_t> initialnu_pz;

  //Cross section
  std::vector<Float_t> xsecs;
  std::vector<Double_t> dxsecs;
  std::vector<Int_t> dxsec_vars;

  //Kinematics
  std::vector<Float_t> y;
  std::vector<Double_t> Q2s;
  std::vector<Double_t> Ws;

  //Process info
  std::vector<Int_t> scattering_type;

  //Make TTree
  TFile* file = new TFile("genie_tree.root", "RECREATE");
  TTree *numutree = new TTree("genie_tree","gevgen");

  //Muon
  numutree->Branch("final_mu_E",&final_mu_E);
  numutree->Branch("final_mu_px",&final_mu_px);
  numutree->Branch("final_mu_py",&final_mu_py);
  numutree->Branch("final_mu_pz",&final_mu_pz);

  //Nu
  numutree->Branch("initialnu_pdg",&initialnu_pdg);
  numutree->Branch("initialnu_E",&initialnu_E);
  numutree->Branch("initialnu_px",&initialnu_px);
  numutree->Branch("initialnu_py",&initialnu_py);
  numutree->Branch("initialnu_pz",&initialnu_pz);

  //Cross section
  numutree->Branch("xsecs",&xsecs);
  numutree->Branch("dxsecs",&dxsecs);
  numutree->Branch("dxsec_vars",&dxsec_vars);

  //Kinematics
  numutree->Branch("y",&y);
  numutree->Branch("Q2",&Q2s);
  numutree->Branch("W",&Ws);

  //Process info
  numutree->Branch("scattering_type",&scattering_type); //https://github.com/GENIE-MC/Generator/blob/d93a9d54e0dd3e70ab98e35b528453b43cb0351a/src/Framework/Interaction/ScatteringType.h#L35-L60

  TFile infile(dir+filename);

  // Get the GENIE GHEP tree and set its branch address
  TTree * tree = dynamic_cast<TTree*> ( infile.Get("gtree") );

  NtpMCEventRecord * mcrec = 0;
  tree->SetBranchAddress( "gmcrec", &mcrec);

  // Event loop
  for(Long64_t i=0; i < tree->GetEntries(); i++){
    tree->GetEntry(i);

    EventRecord & event = *( mcrec->event );
    
    const Interaction & inter = *( event.Summary() ) ;

    const ProcessInfo & proc_info = inter.ProcInfo() ;

    //proc_info.IsQuasiElastic() && 
    if (proc_info.IsWeakCC()){ //CCQE
      //GHepParticle * electron = event.HitElectron(); //Find initial electron
      //const TLorentzVector & pe = *( electron -> P4() ) ; //Get its 4 momentum

      GHepParticle * final_mu = event.FinalStatePrimaryLepton(); //Find final muon
      const TLorentzVector & finalp_mu = *(final_mu -> P4()); //Get its 4 momentum

      int init_nupdg = 0;

      // GHepStatus_t initialstatus = GHepStatus(0);
      // GHepStatus_t finalstatus = GHepStatus(1);
      // for (int pdg : nu_pdgs){
      //   if (event.FindParticle(pdg,initialstatus,0) != 0){
      //     GHepParticle * nu = event.FindParticle(pdg,0,0);
      //     init_nupdg = pdg;
      //   }
      //   if (event.FindParticle(pdg,finalstatus,0) != 0){
      //     GHepParticle * finalnu = event.FindParticle(pdg,0,0);
      //     final_nupdg = pdg;
      //   }
      GHepParticle * nu = event.Particle(0); //Initial nu
      const TLorentzVector & pnu = *( nu -> P4() ) ; //Get its 4 momentum

      //Fill vectors
      final_mu_E.push_back(finalp_mu.E());
      final_mu_px.push_back(finalp_mu.Px());
      final_mu_py.push_back(finalp_mu.Py());
      final_mu_pz.push_back(finalp_mu.Pz());

      initialnu_pdg.push_back(nu->Pdg());
      initialnu_E.push_back(pnu.E());
      initialnu_px.push_back(pnu.Px());
      initialnu_py.push_back(pnu.Py());
      initialnu_pz.push_back(pnu.Pz());

      //Fill xsec
      xsecs.push_back(event.XSec());
      dxsecs.push_back(event.DiffXSec());
      dxsec_vars.push_back(event.DiffXSecVars());

      //Fill Kinematics
      y.push_back(inter.Kine().y(true));
      Q2s.push_back(inter.Kine().Q2(true));
      Ws.push_back(inter.Kine().W(true));

      //Fill process info
      scattering_type.push_back(proc_info.ScatteringTypeId());

    }//If is scattering
  }//end event loop
  numutree->Fill();
  file->Write();
  file->Close();
}

// void make_trees(TString filename="gntp.0.ghep.root", TString dir=""){
//   for (int i = 0; i < 8; i++){
//     //check if gntp exists
//     std::string filename = "gntp.prism_bin"+to_string(i)+".ghep.root";
//     if (gSystem->AccessPathName(filename.c_str())){
//       std::cout << "File " << filename << " does not exist" << std::endl;
//       continue;
//     }
//     make_tree(filename);
//     gSystem->Rename("genie_tree.root", ("genie_tree_prism_bin"+to_string(i)+".root").c_str());
//   }
// }

void make_trees_folder(TString filename="gntp.0.ghep.root", std::string dir=""){
  std::vector<std::string> folders = {"seed420", "seed421", "seed422", "seed423", "seed424", "seed425", "seed426", "seed427", "seed428", "seed429"};

  for (const auto& folder : folders){
    std::string filename = "gntp.0.ghep.root";
    std::string full_dir = dir + folder + "/";

    //check if gntp exists
    if (gSystem->AccessPathName((full_dir + filename).c_str())){
      std::cout << "File " << full_dir + filename << " does not exist" << std::endl;
      continue;
    }
    std::cout<<"Making tree for "<<full_dir + filename<<std::endl;
    make_tree(filename, full_dir);
    gSystem->Rename("genie_tree.root", (full_dir + "genie_tree.root").c_str());
  }
}