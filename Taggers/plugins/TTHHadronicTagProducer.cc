#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDMException.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "flashgg/DataFormats/interface/Jet.h"
#include "flashgg/DataFormats/interface/DiPhotonCandidate.h"
#include "flashgg/DataFormats/interface/TTHHadronicTag.h"
#include "flashgg/DataFormats/interface/DiPhotonMVAResult.h"
#include "flashgg/DataFormats/interface/Electron.h"
#include "flashgg/DataFormats/interface/Muon.h"

#include "flashgg/Taggers/interface/LeptonSelection2018.h"
#include "flashgg/DataFormats/interface/Met.h"

#include "DataFormats/Math/interface/deltaR.h"

#include "flashgg/DataFormats/interface/TagTruthBase.h"

#include "DataFormats/Common/interface/RefToPtr.h"
#include "SimDataFormats/HTXS/interface/HiggsTemplateCrossSections.h"

#include "flashgg/Taggers/src/BDT_resolvedTopTagger.C"

#include <vector>
#include <algorithm>
#include <string>
#include <utility>

#include "TMath.h"
#include "TMVA/Reader.h"
#include "TRandom.h"

TRandom* myRandHadronic = new TRandom();


using namespace std;
using namespace edm;

namespace flashgg {

    class TTHHadronicTagProducer : public EDProducer
    {

    public:
        typedef math::XYZPoint Point;

        TTHHadronicTagProducer( const ParameterSet & );
    private:
        void produce( Event &, const EventSetup & ) override;
        int  chooseCategory( float );

        std::vector<edm::EDGetTokenT<View<flashgg::Jet> > > tokenJets_;
        EDGetTokenT<View<DiPhotonCandidate> > diPhotonToken_;
        std::vector<edm::InputTag> inputTagJets_;
        EDGetTokenT<View<Electron> > electronToken_;
        EDGetTokenT<View<flashgg::Muon> > muonToken_;
        EDGetTokenT<View<reco::Vertex> > vertexToken_;
        EDGetTokenT<View<DiPhotonMVAResult> > mvaResultToken_;
        EDGetTokenT<View<flashgg::Met> > METToken_;
        EDGetTokenT<View<reco::GenParticle> > genParticleToken_;
        EDGetTokenT<int> stage0catToken_, stage1catToken_, njetsToken_;
        EDGetTokenT<HTXS::HiggsClassification> newHTXSToken_;
        EDGetTokenT<float> pTHToken_,pTVToken_;
        EDGetTokenT<double> rhoTag_;
        string systLabel_;


        typedef std::vector<edm::Handle<edm::View<flashgg::Jet> > > JetCollectionVector;
        bool useTTHHadronicMVA_;

        //---thresholds---
        //---photons
        double MVAThreshold_;
        double MVATTHHMVAThreshold_;
        double PhoMVAThreshold_;
        double leadPhoPtThreshold_;
        bool   leadPhoUseVariableTh_;
        double leadPhoOverMassThreshold_;
        double leadPhoOverMassTTHHMVAThreshold_;
        double subleadPhoPtThreshold_;
        bool   subleadPhoUseVariableTh_;
        double subleadPhoOverMassThreshold_;
        //---jets
        double jetPtThreshold_;
        double jetEtaThreshold_;
        double dRJetPhoLeadCut_;
        double dRJetPhoSubleadCut_;
        vector<double> bDiscriminator_;
        double jetsNumberThreshold_;
        double bjetsNumberThreshold_;
        double bjetsLooseNumberThreshold_;
        double jetsNumberTTHHMVAThreshold_;
        double bjetsNumberTTHHMVAThreshold_;
        double bjetsLooseNumberTTHHMVAThreshold_;
        double secondMaxBTagTTHHMVAThreshold_;
        string bTag_;
        //leptons

        double MuonEtaCut_;
        double MuonPtCut_;
        double MuonIsoCut_;
        double MuonPhotonDrCut_;

        double ElePtCut_;
        std::vector<double> EleEtaCuts_;
        double ElePhotonDrCut_;
        double ElePhotonZMassCut_;
        double DeltaRTrkEle_;
        bool debug_;

        unique_ptr<TMVA::Reader>TThMva_;
        FileInPath tthMVAweightfile_;
        string _MVAMethod;

        int jetcount_;
        float nJets_;
        int njets_btagloose_;
        int njets_btagmedium_;
        int njets_btagtight_;
        double idmva1_;
        double idmva2_;
        float leadJetPt_;
        float leadJetBTag_;
        float subLeadJetPt_;
        float sumJetPt_;
        float maxBTagVal_;
        float secondMaxBTagVal_;
        float thirdMaxBTagVal_;
        float fourthMaxBTagVal_;

        float mindRPhoLeadJet_;
        float maxdRPhoLeadJet_;

        float minPhoID_;
        float maxPhoID_;
        float pho1_ptoM_;
        float pho2_ptoM_;
        float pho1_sceta_;
        float pho2_sceta_;
        float pho1_sigmaEOverE_;
        float pho2_sigmaEOverE_;
        float pho1_scphi_;
        float pho2_scphi_;
        float pho1_hasPixelSeed_;
        float pho2_hasPixelSeed_;
        float diPhoY_;
        float diPhoPtoM_;
        float diPhoCosPhi_;
        float nbloose_;

        float btag_1_;
        float jetPt_1_;
        float jetEta_1_;
        float jetPhi_1_;
        float btag_2_;
        float jetPt_2_;
        float jetEta_2_;
        float jetPhi_2_;
        float btag_3_;
        float jetPt_3_;
        float jetEta_3_;
        float jetPhi_3_;
        float btag_4_;
        float jetPt_4_;
        float jetEta_4_;
        float jetPhi_4_;
      
      float MET_;
      
        float tthMvaVal_;

        vector<double> boundaries;

      BDT_resolvedTopTagger *topTagger;
    };

    TTHHadronicTagProducer::TTHHadronicTagProducer( const ParameterSet &iConfig ) :
        diPhotonToken_( consumes<View<flashgg::DiPhotonCandidate> >( iConfig.getParameter<InputTag> ( "DiPhotonTag" ) ) ),
        inputTagJets_( iConfig.getParameter<std::vector<edm::InputTag> >( "inputTagJets" ) ),
        electronToken_( consumes<View<flashgg::Electron> >( iConfig.getParameter<InputTag>( "ElectronTag" ) ) ),
        muonToken_( consumes<View<flashgg::Muon> >( iConfig.getParameter<InputTag>( "MuonTag" ) ) ),
        vertexToken_( consumes<View<reco::Vertex> >( iConfig.getParameter<InputTag> ( "VertexTag" ) ) ),
        mvaResultToken_( consumes<View<flashgg::DiPhotonMVAResult> >( iConfig.getParameter<InputTag>( "MVAResultTag" ) ) ),
        METToken_( consumes<View<flashgg::Met> >( iConfig.getParameter<InputTag> ( "METTag" ) ) ),
        genParticleToken_( consumes<View<reco::GenParticle> >( iConfig.getParameter<InputTag> ( "GenParticleTag" ) ) ),
        rhoTag_( consumes<double>( iConfig.getParameter<InputTag>( "rhoTag" ) ) ),
        systLabel_( iConfig.getParameter<string> ( "SystLabel" ) ),
        _MVAMethod( iConfig.getParameter<string> ( "MVAMethod" ) )
    {
        boundaries = iConfig.getParameter<vector<double > >( "Boundaries" );
        assert( is_sorted( boundaries.begin(), boundaries.end() ) ); // 

        ParameterSet HTXSps = iConfig.getParameterSet( "HTXSTags" );
        stage0catToken_ = consumes<int>( HTXSps.getParameter<InputTag>("stage0cat") );
        stage1catToken_ = consumes<int>( HTXSps.getParameter<InputTag>("stage1cat") );
        njetsToken_ = consumes<int>( HTXSps.getParameter<InputTag>("njets") );
        pTHToken_ = consumes<float>( HTXSps.getParameter<InputTag>("pTH") );
        pTVToken_ = consumes<float>( HTXSps.getParameter<InputTag>("pTV") );
        newHTXSToken_ = consumes<HTXS::HiggsClassification>( HTXSps.getParameter<InputTag>("ClassificationObj") );


        MVAThreshold_ = iConfig.getParameter<double>( "MVAThreshold");
        MVATTHHMVAThreshold_ = iConfig.getParameter<double>( "MVATTHHMVAThreshold");
        PhoMVAThreshold_ = iConfig.getParameter<double>( "PhoMVAThreshold");

        leadPhoPtThreshold_ = iConfig.getParameter<double>( "leadPhoPtThreshold");
        leadPhoUseVariableTh_ = iConfig.getParameter<bool>( "leadPhoUseVariableThreshold");
        leadPhoOverMassThreshold_ = iConfig.getParameter<double>( "leadPhoOverMassThreshold");
        leadPhoOverMassTTHHMVAThreshold_ = iConfig.getParameter<double>( "leadPhoOverMassTTHHMVAThreshold");
        subleadPhoPtThreshold_ = iConfig.getParameter<double>( "subleadPhoPtThreshold");
        subleadPhoUseVariableTh_ = iConfig.getParameter<bool>( "subleadPhoUseVariableThreshold");
        subleadPhoOverMassThreshold_ = iConfig.getParameter<double>( "subleadPhoOverMassThreshold");
        jetPtThreshold_ = iConfig.getParameter<double>( "jetPtThreshold");
        jetEtaThreshold_ = iConfig.getParameter<double>( "jetEtaThreshold");
        dRJetPhoLeadCut_ = iConfig.getParameter<double>( "dRJetPhoLeadCut");
        dRJetPhoSubleadCut_ = iConfig.getParameter<double>( "dRJetPhoSubleadCut");
        bDiscriminator_ = iConfig.getParameter<vector<double > >( "bDiscriminator");
        jetsNumberThreshold_ = iConfig.getParameter<int>( "jetsNumberThreshold");
        bjetsNumberThreshold_ = iConfig.getParameter<int>( "bjetsNumberThreshold");
        bTag_ = iConfig.getParameter<string> ( "bTag");

        MuonEtaCut_ = iConfig.getParameter<double>( "MuonEtaCut");
        MuonPtCut_ = iConfig.getParameter<double>( "MuonPtCut");
        MuonIsoCut_ = iConfig.getParameter<double>( "MuonIsoCut");
        MuonPhotonDrCut_ = iConfig.getParameter<double>( "MuonPhotonDrCut");
 
        EleEtaCuts_ = iConfig.getParameter<std::vector<double>>( "EleEtaCuts");
        ElePtCut_ = iConfig.getParameter<double>( "ElePtCut");
        ElePhotonDrCut_ = iConfig.getParameter<double>( "ElePhotonDrCut");
        ElePhotonZMassCut_ = iConfig.getParameter<double>( "ElePhotonZMassCut");
        DeltaRTrkEle_ = iConfig.getParameter<double>( "DeltaRTrkEle");

        debug_ = iConfig.getParameter<bool>( "debug" );

        useTTHHadronicMVA_ = iConfig.getParameter<bool>( "useTTHHadronicMVA");

        bjetsLooseNumberThreshold_ = iConfig.getParameter<int>( "bjetsLooseNumberThreshold");
        jetsNumberTTHHMVAThreshold_ = iConfig.getParameter<int>( "jetsNumberTTHHMVAThreshold");
        bjetsNumberTTHHMVAThreshold_ = iConfig.getParameter<int>( "bjetsNumberTTHHMVAThreshold");
        bjetsLooseNumberTTHHMVAThreshold_ = iConfig.getParameter<int>( "bjetsLooseNumberTTHHMVAThreshold");
        secondMaxBTagTTHHMVAThreshold_ = iConfig.getParameter<double>( "secondMaxBTagTTHHMVAThreshold");

  
        tthMVAweightfile_ = iConfig.getParameter<edm::FileInPath>( "tthMVAweightfile" ); 

        nJets_ = 0;
        leadJetPt_ = 0.;
        leadJetBTag_ = -1.;
        subLeadJetPt_ = 0.;
        sumJetPt_ = 0.;

        maxBTagVal_ = -3.;
        secondMaxBTagVal_ = -3.;
        thirdMaxBTagVal_ = -3.;
        fourthMaxBTagVal_ = -3.;

        mindRPhoLeadJet_ = -999;
        maxdRPhoLeadJet_= -999;

        minPhoID_= -999.;
        maxPhoID_= -999.;
        pho1_ptoM_= -999.;
        pho2_ptoM_= -999.;
        pho1_sceta_= -999.;
        pho2_sceta_= -999.;
        pho1_scphi_= -999.;
        pho2_scphi_= -999.;
        pho1_sigmaEOverE_= -999.;
        pho2_sigmaEOverE_= -999.;

	pho1_hasPixelSeed_=-999;
        pho2_hasPixelSeed_=-999;

        diPhoY_= -999.;
        diPhoPtoM_= -999.;
        diPhoCosPhi_= -999.;
        nbloose_=-999;

        btag_1_=-1;
        jetPt_1_=-1;
        jetEta_1_=-6;
        jetPhi_1_=-6;
        btag_2_=-1;
        jetPt_2_=-1;
        jetEta_2_=-6;
        jetPhi_2_=-6;
        btag_3_=-1;
        jetPt_3_=-1;
        jetEta_3_=-6;
        jetPhi_3_=-6;
        btag_4_=-1;
        jetPt_4_=-1;
        jetEta_4_=-6;
        jetPhi_4_=-6;
                
	MET_=-1;

        if (_MVAMethod != ""){
            TThMva_.reset( new TMVA::Reader( "!Color:Silent" ) );

            TThMva_->AddVariable( "nJets", &nJets_);
            TThMva_->AddVariable( "sumJetPt", &sumJetPt_);
            TThMva_->AddVariable( "maxBTagVal",&maxBTagVal_);
            TThMva_->AddVariable( "secondMaxBTagVal", &secondMaxBTagVal_);
            TThMva_->AddVariable( "pho1_ptoM", &pho1_ptoM_);
            TThMva_->AddVariable( "pho2_ptoM", &pho2_ptoM_);
            TThMva_->AddVariable( "pho1_sceta", &pho1_sceta_);
            TThMva_->AddVariable( "pho2_sceta", &pho2_sceta_);
            TThMva_->AddVariable( "pho1_scphi", &pho1_scphi_);
            TThMva_->AddVariable( "pho2_scphi", &pho2_scphi_);
            TThMva_->AddVariable( "diPhoY", &diPhoY_);

            TThMva_->AddVariable( "minPhoID", &minPhoID_);
            TThMva_->AddVariable( "maxPhoID", &maxPhoID_);
            TThMva_->AddVariable( "diPhoPtoM", &diPhoPtoM_);  

            TThMva_->AddVariable( "btag_1", &btag_1_);       
            TThMva_->AddVariable( "btag_2", &btag_2_);     
            TThMva_->AddVariable( "btag_3", &btag_3_);   
            TThMva_->AddVariable( "btag_4", &btag_4_);    
            TThMva_->AddVariable( "jetPt_1", &jetPt_1_);      
            TThMva_->AddVariable( "jetPt_2", &jetPt_2_);     
            TThMva_->AddVariable( "jetPt_3", &jetPt_3_);     
            TThMva_->AddVariable( "jetPt_4", &jetPt_4_);   
            TThMva_->AddVariable( "jetEta_1", &jetEta_1_);   
            TThMva_->AddVariable( "jetEta_2", &jetEta_2_);   
            TThMva_->AddVariable( "jetEta_3", &jetEta_3_);   

            TThMva_->AddVariable( "jetEta_4", &jetEta_4_);       
	    TThMva_->AddVariable( "pho1_hasPixelSeed",&pho1_hasPixelSeed_);
	    TThMva_->AddVariable( "pho2_hasPixelSeed",&pho2_hasPixelSeed_);
            TThMva_->AddVariable( "thirdMaxBTagVal", &thirdMaxBTagVal_);          
	    TThMva_->AddVariable( "MET",&MET_);

            TThMva_->BookMVA( _MVAMethod.c_str() , tthMVAweightfile_.fullPath() );
        
        }       

        for (unsigned i = 0 ; i < inputTagJets_.size() ; i++) {
            auto token = consumes<View<flashgg::Jet> >(inputTagJets_[i]);
            tokenJets_.push_back(token);
        }

        produces<vector<TTHHadronicTag> >();
        produces<vector<TagTruthBase> >();

	topTagger = new BDT_resolvedTopTagger("/home/users/hmei/ttH/BabyMaker/CMSSW_9_4_6/src/flashgg/Taggers/plugins/resTop_xgb_csv_order_deepCTag.xml");
    }

    int TTHHadronicTagProducer::chooseCategory( float tthmvavalue )
    {
        // should return 0 if mva above all the numbers, 1 if below the first, ..., boundaries.size()-N if below the Nth, ...
        int n;
        for( n = 0 ; n < ( int )boundaries.size() ; n++ ) {
            if( ( double )tthmvavalue > boundaries[boundaries.size() - n - 1] ) { return n; }
        }
        return -1; // Does not pass, object will not be produced
    }

    void TTHHadronicTagProducer::produce( Event &evt, const EventSetup & )
    {

        Handle<int> stage0cat, stage1cat, njets;
        Handle<float> pTH, pTV;
        evt.getByToken(stage0catToken_, stage0cat);
        evt.getByToken(stage1catToken_,stage1cat);
        evt.getByToken(njetsToken_,njets);
        evt.getByToken(pTHToken_,pTH);
        evt.getByToken(pTVToken_,pTV);
        Handle<HTXS::HiggsClassification> htxsClassification;
        evt.getByToken(newHTXSToken_,htxsClassification);


        //Handle<View<flashgg::Jet> > theJets;
        //evt.getByToken( thejetToken_, theJets );
        // const PtrVector<flashgg::Jet>& jetPointers = theJets->ptrVector();
        JetCollectionVector Jets( inputTagJets_.size() );
        for( size_t j = 0; j < inputTagJets_.size(); ++j ) {
            evt.getByToken( tokenJets_[j], Jets[j] );
        }

        Handle<View<flashgg::DiPhotonCandidate> > diPhotons;
        evt.getByToken( diPhotonToken_, diPhotons );
        // const PtrVector<flashgg::DiPhotonCandidate>& diPhotonPointers = diPhotons->ptrVector();

        Handle<View<flashgg::Muon> > theMuons;
        evt.getByToken( muonToken_, theMuons );

        Handle<View<flashgg::Electron> > theElectrons;
        evt.getByToken( electronToken_, theElectrons );

        edm::Handle<double>  rho;
        evt.getByToken(rhoTag_,rho);
        double rho_    = *rho;

        Handle<View<reco::Vertex> > vertices;
        evt.getByToken( vertexToken_, vertices );

        Handle<View<flashgg::DiPhotonMVAResult> > mvaResults;
        evt.getByToken( mvaResultToken_, mvaResults );
        // const PtrVector<flashgg::DiPhotonMVAResult>& mvaResultPointers = mvaResults->ptrVector();

        Handle<View<flashgg::Met> > METs;
        evt.getByToken( METToken_, METs );

        Handle<View<reco::GenParticle> > genParticles;

        std::unique_ptr<vector<TTHHadronicTag> > tthhtags( new vector<TTHHadronicTag> );
        std::unique_ptr<vector<TagTruthBase> > truths( new vector<TagTruthBase> );

        Point higgsVtx;
        if( ! evt.isRealData() ) {
            evt.getByToken( genParticleToken_, genParticles );
            /*
            for( unsigned int genLoop = 0 ; genLoop < genParticles->size(); genLoop++ ) {
                int pdgid = genParticles->ptrAt( genLoop )->pdgId();
                if( pdgid == 25 ){
                    cout << "Higgs found " <<endl;
                    if ( genParticles->ptrAt( genLoop )->numberOfDaughters() == 2 ){ 
                        const reco::Candidate * d1 =  genParticles->ptrAt( genLoop )->daughter( 0 );
                        const reco::Candidate * d2 =  genParticles->ptrAt( genLoop )->daughter( 1 );
                        cout << "Higgs with status = " <<  genParticles->ptrAt( genLoop )->status() << " has two daughters with pdgId: " << endl;
                        cout << "d1 pdgId = " << d1->pdgId() << "   d2 pdgId = "<< d2->pdgId() <<endl;
                    }
                }
            }
            */
                //if (d1->pdgId()!=22 || d2->pdgId()!=22) continue;
                
            for( unsigned int genLoop = 0 ; genLoop < genParticles->size(); genLoop++ ) {
                int pdgid = genParticles->ptrAt( genLoop )->pdgId();
                
                if( pdgid == 25 || pdgid == 22 ) {
                    higgsVtx = genParticles->ptrAt( genLoop )->vertex();           
                    break;
                }
            }
        }
        
        edm::RefProd<vector<TagTruthBase> > rTagTruth = evt.getRefBeforePut<vector<TagTruthBase> >();
        unsigned int idx = 0;
        
        
        for( unsigned int diphoIndex = 0; diphoIndex < diPhotons->size(); diphoIndex++ ) {

            edm::Ptr<flashgg::DiPhotonCandidate> dipho = diPhotons->ptrAt( diphoIndex );

            std::vector<edm::Ptr<flashgg::Muon> >     Muons;
            std::vector<edm::Ptr<flashgg::Electron> > Electrons;

            if(theMuons->size()>0)
                Muons = selectMuons(theMuons->ptrs(), dipho, vertices->ptrs(), MuonPtCut_, MuonEtaCut_, MuonIsoCut_, MuonPhotonDrCut_, debug_);
            if(theElectrons->size()>0)
                Electrons = selectElectrons(theElectrons->ptrs(), dipho, ElePtCut_, EleEtaCuts_, ElePhotonDrCut_, ElePhotonZMassCut_, DeltaRTrkEle_, debug_);

            if( (Muons.size() + Electrons.size()) != 0) continue;

            jetcount_ = 0;

            nJets_ = 0;
            njets_btagloose_ = 0;
            njets_btagmedium_ = 0;
            njets_btagtight_ = 0;

            idmva1_ = -999.;
            idmva2_ = -999.;

            leadJetPt_ = 0.;
            leadJetBTag_ = -1.;
            subLeadJetPt_ = 0.;
            sumJetPt_ = 0.;

            maxBTagVal_ = -3.;
            secondMaxBTagVal_ = -3.;
            thirdMaxBTagVal_ = -3.;
            fourthMaxBTagVal_ = -3.;
	    
	    mindRPhoLeadJet_ = -999;
	    maxdRPhoLeadJet_= -999;
	    
	    minPhoID_= -999.;
	    maxPhoID_= -999.;
	    pho1_ptoM_= -999.;
	    pho2_ptoM_= -999.;
	    pho1_sceta_= -999.;
	    pho2_sceta_= -999.;
	    pho1_scphi_= -999.;
	    pho2_scphi_= -999.;
	    pho1_hasPixelSeed_=-999;
	    pho2_hasPixelSeed_=-999;

	    pho1_sigmaEOverE_= -999.;
	    pho2_sigmaEOverE_= -999.;
	    diPhoY_= -999.;
	    diPhoPtoM_= -999.;
	    diPhoCosPhi_= -999.;
	    nbloose_=-999;

	    btag_1_=-1;
	    jetPt_1_=-1;
	    jetEta_1_=-6;
	    jetPhi_1_=-6;
	    btag_2_=-1;
	    jetPt_2_=-1;
	    jetEta_2_=-6;
	    jetPhi_2_=-6;
	    btag_3_=-1;
	    jetPt_3_=-1;
	    jetEta_3_=-6;
	    jetPhi_3_=-6;
	    btag_4_=-1;
	    jetPt_4_=-1;
	    jetEta_4_=-6;
	    jetPhi_4_=-6;
                
	    MET_=-1;
	    
            tthMvaVal_ = -999.;


            unsigned int jetCollectionIndex = diPhotons->ptrAt( diphoIndex )->jetCollectionIndex();

            std::vector<edm::Ptr<flashgg::Jet> > JetVect;
            JetVect.clear();
            std::vector<edm::Ptr<flashgg::Jet> > BJetVect;
            BJetVect.clear();
            std::vector<edm::Ptr<flashgg::Jet> > BJetTTHHMVAVect;
            BJetTTHHMVAVect.clear();
            
            std::vector<edm::Ptr<reco::GenJet> > genJetVect;
            genJetVect.clear();

            std::vector<float> JetBTagVal;
            JetBTagVal.clear();
        
            idmva1_ = dipho->leadingPhoton()->phoIdMvaDWrtVtx( dipho->vtx() );
            idmva2_ = dipho->subLeadingPhoton()->phoIdMvaDWrtVtx( dipho->vtx() );

            if( idmva1_ <= PhoMVAThreshold_ || idmva2_ <= PhoMVAThreshold_ ) { continue; }

            edm::Ptr<flashgg::DiPhotonMVAResult> mvares = mvaResults->ptrAt( diphoIndex );
        
            if( !dipho->leadingPhoton()->passElectronVeto() || !dipho->subLeadingPhoton()->passElectronVeto() ) { continue; }

            double leadPhoPtCut = leadPhoPtThreshold_;
            double subleadPhoPtCut = subleadPhoPtThreshold_;
            if( leadPhoUseVariableTh_ )
            { 
                leadPhoPtCut = leadPhoOverMassThreshold_ * dipho->mass(); 
                if(useTTHHadronicMVA_){
                    leadPhoPtCut = leadPhoOverMassTTHHMVAThreshold_ * dipho->mass();
                }
            }
            if( subleadPhoUseVariableTh_ )
            { subleadPhoPtCut = subleadPhoOverMassThreshold_ * dipho->mass(); }
            double diphoMVAcut = MVAThreshold_;
            if(useTTHHadronicMVA_){
                    diphoMVAcut = MVATTHHMVAThreshold_;
            }

            if( dipho->leadingPhoton()->pt() < leadPhoPtCut || dipho->subLeadingPhoton()->pt() < subleadPhoPtCut ) { continue; }
            if( mvares->mvaValue() < diphoMVAcut ) { continue; }

	    //BDT_resolvedTopTagger topTagger("/home/users/hmei/ttH/BabyMaker/CMSSW_9_4_6/src/flashgg/Taggers/plugins/resTop_xgb_csv_order_deepCTag.xml");                                            
	    cout << "njets: " << Jets[jetCollectionIndex]->size() << endl;
            for( unsigned int jetIndex = 0; jetIndex < Jets[jetCollectionIndex]->size() ; jetIndex++ ) {
                edm::Ptr<flashgg::Jet> thejet = Jets[jetCollectionIndex]->ptrAt( jetIndex );
                if( fabs( thejet->eta() ) > jetEtaThreshold_ ) { continue; }
                if(!thejet->passesJetID  ( flashgg::Tight2017 ) ) { continue; }
                float dRPhoLeadJet = deltaR( thejet->eta(), thejet->phi(), dipho->leadingPhoton()->superCluster()->eta(), dipho->leadingPhoton()->superCluster()->phi() ) ;
                float dRPhoSubLeadJet = deltaR( thejet->eta(), thejet->phi(), dipho->subLeadingPhoton()->superCluster()->eta(),
                                                dipho->subLeadingPhoton()->superCluster()->phi() );

                if( dRPhoLeadJet < dRJetPhoLeadCut_ || dRPhoSubLeadJet < dRJetPhoSubleadCut_ ) { continue; }
                if( thejet->pt() < jetPtThreshold_ ) { continue; }

                jetcount_++;
                nJets_ = jetcount_;
                JetVect.push_back( thejet );
                
                //genJetVect.push_back( thejet->genJet());
                //cout<<"TTH Jet "<< jetcount_<<" Pt:"<<thejet->pt()<<" genPt:"<<thejet->genJet()->pt()<<" hflav: "<<thejet->hadronFlavour()<<" pFlav:"<<thejet->partonFlavour()<< endl;

                float bDiscriminatorValue = -2.;
                if(bTag_ == "pfDeepCSV") bDiscriminatorValue = thejet->bDiscriminator("pfDeepCSVJetTags:probb")+thejet->bDiscriminator("pfDeepCSVJetTags:probbb") ;
                else  bDiscriminatorValue = thejet->bDiscriminator( bTag_ );

                float cvsl = thejet->bDiscriminator("pfDeepCSVJetTags:probc") + thejet->bDiscriminator("pfDeepCSVJetTags:probudsg") ;
                float cvsb = thejet->bDiscriminator("pfDeepCSVJetTags:probc") + thejet->bDiscriminator("pfDeepCSVJetTags:probb")+thejet->bDiscriminator("pfDeepCSVJetTags:probbb") ;
                float ptD = thejet->userFloat("ptD") ;
                float axis1 = thejet->userFloat("axis1") ;
                int mult = thejet->userFloat("totalMult") ;

                topTagger->addJet(thejet->pt(), thejet->eta(), thejet->phi(), thejet->mass(), bDiscriminatorValue, cvsl, cvsb, ptD, axis1, mult);            
               

                float jetPt = thejet->pt();
                if(jetPt > leadJetPt_){
                    if(leadJetPt_ > subLeadJetPt_) { subLeadJetPt_ = leadJetPt_; }
                    leadJetPt_ = jetPt;
                    leadJetBTag_=  bDiscriminatorValue;
                } else if(jetPt > subLeadJetPt_){
                    subLeadJetPt_ = jetPt;
                }
                sumJetPt_ += jetPt;
                
                
                if(bDiscriminatorValue > maxBTagVal_){ 

                    BJetTTHHMVAVect.insert( BJetTTHHMVAVect.begin(), thejet );
                    if(BJetTTHHMVAVect.size() >= 3){ BJetTTHHMVAVect.pop_back(); }
                    
                    if(thirdMaxBTagVal_>fourthMaxBTagVal_) { fourthMaxBTagVal_= thirdMaxBTagVal_;}
                    if(secondMaxBTagVal_>thirdMaxBTagVal_){ thirdMaxBTagVal_= secondMaxBTagVal_; }
                    if(maxBTagVal_ > secondMaxBTagVal_) { secondMaxBTagVal_ = maxBTagVal_; }

                    maxBTagVal_ = bDiscriminatorValue;

                } else if(bDiscriminatorValue > secondMaxBTagVal_){

                    if(BJetTTHHMVAVect.size() >= 2){BJetTTHHMVAVect.pop_back();} 
                    BJetTTHHMVAVect.push_back( thejet );

                    if(thirdMaxBTagVal_>fourthMaxBTagVal_) { fourthMaxBTagVal_= thirdMaxBTagVal_;}
                    if(secondMaxBTagVal_>thirdMaxBTagVal_) { thirdMaxBTagVal_= secondMaxBTagVal_;}
                    secondMaxBTagVal_ = bDiscriminatorValue;

                }else if(bDiscriminatorValue > thirdMaxBTagVal_){

                    if(thirdMaxBTagVal_>fourthMaxBTagVal_) { fourthMaxBTagVal_= thirdMaxBTagVal_;}
                    thirdMaxBTagVal_ = bDiscriminatorValue;

                }else if(bDiscriminatorValue > fourthMaxBTagVal_){
                   fourthMaxBTagVal_ = bDiscriminatorValue;
                }

                JetBTagVal.push_back( bDiscriminatorValue );

                if( bDiscriminatorValue > bDiscriminator_[0] ) njets_btagloose_++;
                if( bDiscriminatorValue > bDiscriminator_[1] ){
                    
                    njets_btagmedium_++;
                    BJetVect.push_back( thejet );
                }
                if( bDiscriminatorValue > bDiscriminator_[2] ) njets_btagtight_++;
            }
        
            if( METs->size() != 1 ) { std::cout << "WARNING - #MET is not 1" << std::endl;}
            Ptr<flashgg::Met> theMET = METs->ptrAt( 0 );

            vector<float> mvaEval = topTagger->EvalMVA();                                                                                        
	    cout << "eval top tagger: " << mvaEval.size() << endl;                                                                                                                   
            for (unsigned topt = 0; topt < mvaEval.size(); topt++)                                                                                                                                         
              if (topt < mvaEval.size() - 1) cout << mvaEval[topt] << ", ";                                                                                                                               
              else cout << mvaEval[topt];                                                                                                                                                                  
            cout << endl;               
	    topTagger->clear();
            if( METs->size() != 1 ) { std::cout << "WARNING - #MET is not 1" << std::endl;}

            if(useTTHHadronicMVA_){

                BJetVect.clear();
                BJetVect = BJetTTHHMVAVect;

                if(JetVect.size()>1) mindRPhoLeadJet_=TMath::Min(deltaR( dipho->leadingPhoton()->eta(),dipho->leadingPhoton()->phi(), JetVect[0]->eta(),JetVect[0]->phi()) ,
                                                                deltaR( dipho->leadingPhoton()->eta(),dipho->leadingPhoton()->phi(), JetVect[1]->eta(),JetVect[1]->phi()));
                if(JetVect.size()>1) maxdRPhoLeadJet_=TMath::Max(deltaR( dipho->leadingPhoton()->eta(),dipho->leadingPhoton()->phi(), JetVect[0]->eta(),JetVect[0]->phi()) ,
                                                                deltaR( dipho->leadingPhoton()->eta(),dipho->leadingPhoton()->phi(), JetVect[1]->eta(),JetVect[1]->phi()));

                minPhoID_=TMath::Min( idmva1_, idmva2_);
                maxPhoID_=TMath::Max( idmva1_, idmva2_);
                pho1_ptoM_= dipho->leadingPhoton()->pt()/dipho->mass();
                pho2_ptoM_= dipho->subLeadingPhoton()->pt()/dipho->mass();
                pho1_sceta_= dipho->leadingPhoton()->superCluster()->eta();
                pho2_sceta_= dipho->subLeadingPhoton()->superCluster()->eta();
                pho1_scphi_= dipho->leadingPhoton()->superCluster()->phi();
                pho2_scphi_= dipho->subLeadingPhoton()->superCluster()->phi();

                pho1_hasPixelSeed_= dipho->leadingPhoton()->hasPixelSeed();
                pho2_hasPixelSeed_= dipho->subLeadingPhoton()->hasPixelSeed();

                pho1_sigmaEOverE_= dipho->leadingPhoton()->sigEOverE();
                pho2_sigmaEOverE_= dipho->subLeadingPhoton()->sigEOverE();

                diPhoY_= dipho->rapidity();
                diPhoPtoM_= dipho->pt()/dipho->mass();
                diPhoCosPhi_=  TMath::Cos( deltaPhi( dipho->leadingPhoton()->phi(), dipho->subLeadingPhoton()->phi() ) );
                nbloose_=float(njets_btagloose_);
		MET_ = theMET->getCorPt();

                if(JetVect.size()>0){
                    if(bTag_ == "pfDeepCSV") btag_1_=JetVect[0]->bDiscriminator("pfDeepCSVJetTags:probb")+JetVect[0]->bDiscriminator("pfDeepCSVJetTags:probbb") ;
                    else  btag_1_ = JetVect[0]->bDiscriminator( bTag_ );
                    jetPt_1_=JetVect[0]->pt();
                    jetEta_1_=JetVect[0]->eta();
                    jetPhi_1_=JetVect[0]->phi();
                }

                if(JetVect.size()>1){
                    if(bTag_ == "pfDeepCSV") btag_2_=JetVect[1]->bDiscriminator("pfDeepCSVJetTags:probb")+JetVect[1]->bDiscriminator("pfDeepCSVJetTags:probbb") ;
                    else  btag_2_ = JetVect[1]->bDiscriminator( bTag_ );
                    jetPt_2_=JetVect[1]->pt();
                    jetEta_2_=JetVect[1]->eta();
                    jetPhi_2_=JetVect[1]->phi();
                }

                if(JetVect.size()>2){
                    if(bTag_ == "pfDeepCSV") btag_3_=JetVect[2]->bDiscriminator("pfDeepCSVJetTags:probb")+JetVect[2]->bDiscriminator("pfDeepCSVJetTags:probbb") ;
                    else  btag_3_ = JetVect[2]->bDiscriminator( bTag_ );
                    jetPt_3_=JetVect[2]->pt();
                    jetEta_3_=JetVect[2]->eta();
                    jetPhi_3_=JetVect[2]->phi();
                }
                if(JetVect.size()>3){
                    if(bTag_ == "pfDeepCSV") btag_4_=JetVect[3]->bDiscriminator("pfDeepCSVJetTags:probb")+JetVect[3]->bDiscriminator("pfDeepCSVJetTags:probbb") ;
                    else  btag_4_ = JetVect[3]->bDiscriminator( bTag_ );
                    jetPt_4_=JetVect[3]->pt();
                    jetEta_4_=JetVect[3]->eta();
                    jetPhi_4_=JetVect[3]->phi();
                }


                if(JetVect.size()>0){
                    if(bTag_ == "pfDeepCSV") { btag_1_=JetVect[0]->bDiscriminator("pfDeepCSVJetTags:probb")+JetVect[0]->bDiscriminator("pfDeepCSVJetTags:probbb") ;
			//cout << "using addition of prob b and bb" << endl;
		    }
                    else  { btag_1_ = JetVect[0]->bDiscriminator( bTag_ );
			//cout << "using bTag_" << endl; 
		    }
                    jetPt_1_=JetVect[0]->pt();
                    jetEta_1_=JetVect[0]->eta();
                    jetPhi_1_=JetVect[0]->phi();
                }

                if(JetVect.size()>1){
                    if(bTag_ == "pfDeepCSV") btag_2_=JetVect[1]->bDiscriminator("pfDeepCSVJetTags:probb")+JetVect[1]->bDiscriminator("pfDeepCSVJetTags:probbb") ;
                    else  btag_2_ = JetVect[1]->bDiscriminator( bTag_ );
                    jetPt_2_=JetVect[1]->pt();
                    jetEta_2_=JetVect[1]->eta();
                    jetPhi_2_=JetVect[1]->phi();
                }

                if(JetVect.size()>2){
                    if(bTag_ == "pfDeepCSV") btag_3_=JetVect[2]->bDiscriminator("pfDeepCSVJetTags:probb")+JetVect[2]->bDiscriminator("pfDeepCSVJetTags:probbb") ;
                    else  btag_3_ = JetVect[2]->bDiscriminator( bTag_ );
                    jetPt_3_=JetVect[2]->pt();
                    jetEta_3_=JetVect[2]->eta();
                    jetPhi_3_=JetVect[2]->phi();
                }
                if(JetVect.size()>3){
                    if(bTag_ == "pfDeepCSV") btag_4_=JetVect[3]->bDiscriminator("pfDeepCSVJetTags:probb")+JetVect[3]->bDiscriminator("pfDeepCSVJetTags:probbb") ;
                    else  btag_4_ = JetVect[3]->bDiscriminator( bTag_ );
                    jetPt_4_=JetVect[3]->pt();
                    jetEta_4_=JetVect[3]->eta();
                    jetPhi_4_=JetVect[3]->phi();
                }





                if(JetVect.size()>0){
                    if(bTag_ == "pfDeepCSV") { btag_1_=JetVect[0]->bDiscriminator("pfDeepCSVJetTags:probb")+JetVect[0]->bDiscriminator("pfDeepCSVJetTags:probbb") ;
			//cout << "using addition of prob b and bb" << endl;
		    }
                    else  { btag_1_ = JetVect[0]->bDiscriminator( bTag_ );
			//cout << "using bTag_" << endl; 
		    }
                    jetPt_1_=JetVect[0]->pt();
                    jetEta_1_=JetVect[0]->eta();
                    jetPhi_1_=JetVect[0]->phi();
                }

                if(JetVect.size()>1){
                    if(bTag_ == "pfDeepCSV") btag_2_=JetVect[1]->bDiscriminator("pfDeepCSVJetTags:probb")+JetVect[1]->bDiscriminator("pfDeepCSVJetTags:probbb") ;
                    else  btag_2_ = JetVect[1]->bDiscriminator( bTag_ );
                    jetPt_2_=JetVect[1]->pt();
                    jetEta_2_=JetVect[1]->eta();
                    jetPhi_2_=JetVect[1]->phi();
                }

                if(JetVect.size()>2){
                    if(bTag_ == "pfDeepCSV") btag_3_=JetVect[2]->bDiscriminator("pfDeepCSVJetTags:probb")+JetVect[2]->bDiscriminator("pfDeepCSVJetTags:probbb") ;
                    else  btag_3_ = JetVect[2]->bDiscriminator( bTag_ );
                    jetPt_3_=JetVect[2]->pt();
                    jetEta_3_=JetVect[2]->eta();
                    jetPhi_3_=JetVect[2]->phi();
                }
                if(JetVect.size()>3){
                    if(bTag_ == "pfDeepCSV") btag_4_=JetVect[3]->bDiscriminator("pfDeepCSVJetTags:probb")+JetVect[3]->bDiscriminator("pfDeepCSVJetTags:probbb") ;
                    else  btag_4_ = JetVect[3]->bDiscriminator( bTag_ );
                    jetPt_4_=JetVect[3]->pt();
                    jetEta_4_=JetVect[3]->eta();
                    jetPhi_4_=JetVect[3]->phi();
                }

                if(secondMaxBTagVal_ >= secondMaxBTagTTHHMVAThreshold_ && njets_btagloose_ >= bjetsLooseNumberTTHHMVAThreshold_ && njets_btagmedium_ >= bjetsNumberTTHHMVAThreshold_ && jetcount_ >= jetsNumberTTHHMVAThreshold_ && _MVAMethod != ""){
                    
		  if(debug_){
                    cout << "input variables : " << endl;

                    cout << "nJets_ = " << nJets_ <<" jetcount"<< jetcount_<< endl;
		    cout << "sumJetPt_ = " << sumJetPt_ << endl;                    
		    cout << "maxBTagVal_ = " << maxBTagVal_ << endl;
                    cout << "secondMaxBTagVal_ = " << secondMaxBTagVal_ << endl;
                    cout << "thirdMaxBTagVal_ = " << thirdMaxBTagVal_ << endl;
                    //                    cout << "leadJetPt_ = " << leadJetPt_ << endl;
                    //                    cout << "leadJetBTag_ = " << leadJetBTag_ << endl;

                    cout << "minPhoID_ = " << minPhoID_ << endl;
                    cout << "maxPhoID_ = " << maxPhoID_ << endl;
                    //                    cout << "mindRPhoLeadJet_ = " << mindRPhoLeadJet_ << endl;
                    //                    cout << "maxdRPhoLeadJet_ = " << maxdRPhoLeadJet_ << endl;
                    cout << "pho1_ptoM_ = " << pho1_ptoM_ << endl;
                    cout << "pho2_ptoM_ = " << pho2_ptoM_ << endl;
                    cout << "pho1_sceta_ = " << pho1_sceta_ << endl;
                    cout << "pho2_sceta_ = " << pho2_sceta_ << endl;
                    cout << "pho1_scphi_ = " << pho1_scphi_ << endl;
                    cout << "pho2_scphi_ = " << pho2_scphi_ << endl;

                    cout << "pho1_hasPixelSeed_ = " << pho1_hasPixelSeed_ << endl;
                    cout << "pho2_hasPixelSeed_ = " << pho2_hasPixelSeed_ << endl;
		    //                    cout << "pho1_sigmaEoE_ = " << pho1_sigmaEOverE_ << endl;
		    //                    cout << "pho2_sigmaEoE_ = " << pho2_sigmaEOverE_ << endl;

                    cout << "diPhoY_ = " << diPhoY_ << endl;
                    //cout << "diPhoCosPhi_ = " << diPhoCosPhi_ << endl;
                    cout << "diPhoPtoM_ = " << diPhoPtoM_ << endl;

		    //                    cout << "nBLoose_ = " << njets_btagloose_ << " "<<nbloose_<<endl;
                    cout << "btag_1_ = " <<btag_1_ << endl;
                    cout << "jetPt_1_ = " <<jetPt_1_ << endl;
                    cout << "jetEta_1_ = " <<jetEta_1_ << endl;
		    //                    cout << "jetPhi_1_ = " <<jetPhi_1_ << endl;
                    cout << "btag_2_ = " <<btag_2_ << endl;
                    cout << "jetPt_2_ = " <<jetPt_2_ << endl;
                    cout << "jetEta_2_ = " <<jetEta_2_ << endl;
		    //                    cout << "jetPhi_2_ = " <<jetPhi_2_ << endl;
                    cout << "btag_3_ = " <<btag_3_ << endl;
                    cout << "jetPt_3_ = " <<jetPt_3_ << endl;
                    cout << "jetEta_3_ = " <<jetEta_3_ << endl;
		    //                    cout << "jetPhi_3_ = " <<jetPhi_3_ << endl;
                    cout << "btag_4_ = " <<btag_4_ << endl;
                    cout << "jetPt_4_ = " <<jetPt_4_ << endl;
                    cout << "jetEta_4_ = " <<jetEta_4_ << endl;
		    //                    cout << "jetPhi_4_ = " <<jetPhi_4_ << endl;
                    cout << "MET_ = " <<MET_ << endl;
		  }
		  tthMvaVal_ = TThMva_->EvaluateMVA( _MVAMethod.c_str() );

                    //cout << "mva result :" << endl;
                  if(debug_)  cout << "tthMvaVal_ = " << tthMvaVal_  << endl;
                    //cout << "tthMvaVal_ = " << tthMvaVal_  << " "<< boundaries[0]<<" "<< boundaries[1]<< endl;
                     
                 }
            }

            bool isTTHHadronicTagged = false;
            int catnum =-1;
            if( !useTTHHadronicMVA_ && njets_btagloose_ >= bjetsLooseNumberThreshold_ && njets_btagmedium_ >= bjetsNumberThreshold_ && jetcount_ >= jetsNumberThreshold_ ) {

                catnum=0;
                isTTHHadronicTagged = true;
                
            } else if ( useTTHHadronicMVA_  && njets_btagloose_ >= bjetsLooseNumberTTHHMVAThreshold_ && njets_btagmedium_ >= bjetsNumberTTHHMVAThreshold_ && jetcount_ >= jetsNumberTTHHMVAThreshold_ ) {
                //&& tthMvaVal_ >= tthHadMVAThresholdMin_  && tthMvaVal_ < tthHadMVAThresholdMax_ ) {
                
                catnum = chooseCategory( tthMvaVal_ );                
                //                cout<<" catNum="<<catnum<<endl;
                if(catnum>=0){
                    isTTHHadronicTagged = true;
                    //                    cout<<" TAGGED "<< endl;
                }
            }
            
            if( isTTHHadronicTagged ) {

                TTHHadronicTag tthhtags_obj( dipho, mvares, JetVect, BJetVect );
                tthhtags_obj.setCategoryNumber(catnum  );
                tthhtags_obj.setNjet( jetcount_ );
                tthhtags_obj.setNBLoose( njets_btagloose_ );
                tthhtags_obj.setNBMedium( njets_btagmedium_ );
                tthhtags_obj.setNBTight( njets_btagtight_ );
                tthhtags_obj.setDiPhotonIndex( diphoIndex );
                tthhtags_obj.setLeadJetPt( leadJetPt_ );
                tthhtags_obj.setSubLeadJetPt( subLeadJetPt_ );
                tthhtags_obj.setSumJetPt( sumJetPt_ );
                tthhtags_obj.setMaxBTagVal( maxBTagVal_ );
                tthhtags_obj.setSecondMaxBTagVal( secondMaxBTagVal_ );
                tthhtags_obj.setThirdMaxBTagVal( thirdMaxBTagVal_ );
                tthhtags_obj.setFourthMaxBTagVal( fourthMaxBTagVal_ );
                tthhtags_obj.setSystLabel( systLabel_ );
                tthhtags_obj.setMVAres(tthMvaVal_);
                tthhtags_obj.setMET( theMET );

		tthhtags_obj.setDiphoMVARes(mvares->mvaValue());
		tthhtags_obj.setRand(myRandHadronic->Rndm());
		
                tthhtags_obj.setMetPt((float)theMET->pt());
                tthhtags_obj.setMetPhi((float)theMET->phi());

		if (mvaEval.size() > 0) {
		  tthhtags_obj.setTopTagScore(mvaEval[0]);
		  tthhtags_obj.setTopTagTopMass(mvaEval[4]);
		  tthhtags_obj.setTopTagWMass(mvaEval[8]);
		} else
		  {
		  tthhtags_obj.setTopTagScore(-999);
		  tthhtags_obj.setTopTagTopMass(-999);
		  tthhtags_obj.setTopTagWMass(-999);
		  }
		// Gen lepton info
                if( ! evt.isRealData() ) {
                    evt.getByToken( genParticleToken_, genParticles );
                    int nGoodEls(0), nGoodMus(0), nGoodElsFromTau(0), nGoodMusFromTau(0), nGoodTaus(0);
                    cout << "Number of gen particles: " << genParticles->size() << endl;
                    for( unsigned int genLoop = 0 ; genLoop < genParticles->size(); genLoop++ ) {
                        int pdgid = genParticles->ptrAt( genLoop )->pdgId();
                        double pt = genParticles->ptrAt( genLoop )->p4().pt();
                        int status = genParticles->ptrAt( genLoop )->status();
                        bool isPromptFinalState = genParticles->ptrAt( genLoop )->isPromptFinalState();
                        bool isPromptDecayed = genParticles->ptrAt( genLoop )->isPromptDecayed();
                        bool isDirectPromptTauDecayProductFinalState = genParticles->ptrAt( genLoop )->isDirectPromptTauDecayProductFinalState();
                        if (pt < 20) continue;
			/*
                        if (abs(pdgid) == 11 || abs(pdgid) == 13 || abs(pdgid) == 15) {
                            cout << "Found a gen lepton/tau with pT > 20" << endl;
                            cout << "pdgid: " << pdgid << endl;
                            cout << "pt: " << pt << endl;
                            cout << "status: " << status << endl;
                            cout << "isPromptFinalState: " << isPromptFinalState << endl;
                            cout << "isPromptDecayed: " << isPromptDecayed << endl;
                            cout << "isDirectPromptTauDecayProductFinalState: " << isDirectPromptTauDecayProductFinalState << endl;
                        }
			*/
                        if (abs(pdgid) == 11 && status == 1 && (isPromptFinalState || isDirectPromptTauDecayProductFinalState)) {
                            nGoodEls++;
                            if (isDirectPromptTauDecayProductFinalState)
                                nGoodElsFromTau++;
                        } 
                        else if (abs(pdgid) == 13 && status == 1 && (isPromptFinalState || isDirectPromptTauDecayProductFinalState)) {
                            nGoodMus++;
                            if (isDirectPromptTauDecayProductFinalState)
                                nGoodMusFromTau++;
                        }
                        if (abs(pdgid) == 15 && status == 2 && isPromptDecayed) {
                            nGoodTaus++;
                        }
                    } // end gen loop
                    bool lead_photon_is_electron(false), sublead_photon_is_electron(false);
                    double lead_photon_eta = dipho->leadingPhoton()->eta();
                    double lead_photon_phi = dipho->leadingPhoton()->phi();
                    double sublead_photon_eta = dipho->subLeadingPhoton()->eta();
                    double sublead_photon_phi = dipho->subLeadingPhoton()->phi();
                    //double sublead_photon_eta = dipho->subLeadingPhoton()->superCluster()->eta(); // should use subLeadingPhoton()->eta() instead. Not sure why they use supercluster in jet dR matching ...
                    //double sublead_photon_phi = dipho->subLeadingPhoton()->superCluster()->phi();
                    for( unsigned int genLoop = 0 ; genLoop < genParticles->size(); genLoop++ ) {
                        int pdgid = genParticles->ptrAt( genLoop )->pdgId();
                        if (abs(pdgid) != 11) continue;
                        if (genParticles->ptrAt( genLoop )->p4().pt() < 15) continue;
                        if (!genParticles->ptrAt( genLoop )->isPromptFinalState()) continue;
                        double electron_eta = genParticles->ptrAt( genLoop )->p4().eta();
                        double electron_phi = genParticles->ptrAt( genLoop )->p4().phi();

                        cout << "Found an electron with pT: " << genParticles->ptrAt( genLoop )->p4().pt() << endl;


                        double deltaR_lead = sqrt( pow(electron_eta - lead_photon_eta, 2) + pow(electron_phi - lead_photon_phi, 2));
                        double deltaR_sublead = sqrt( pow(electron_eta - sublead_photon_eta, 2) + pow(electron_phi - sublead_photon_phi, 2));

                        cout << "Delta R with leading photon: " << deltaR_lead << endl;
                        cout << "Delta R with subleading photon: " << deltaR_sublead << endl;

                        const double deltaR_thresh = 0.1;
                        if (deltaR_lead < deltaR_thresh)
                            lead_photon_is_electron = true;
                        if (deltaR_sublead < deltaR_thresh)
                            sublead_photon_is_electron = true;

                    } // end gen loop

                    int lead_photon_type;
                    if (dipho->leadingPhoton()->genMatchType() != 1) { // fake
                        if (!lead_photon_is_electron)
                            lead_photon_type = 3;   // fake
                        else
                            lead_photon_type = 2;   // fake from  electron 
                    }
                    else
                        lead_photon_type = 1; // prompt
                    int sublead_photon_type;
                    if (dipho->subLeadingPhoton()->genMatchType() != 1) { // fake
                        if (!sublead_photon_is_electron)
                            sublead_photon_type = 3;   // fake
                        else
                            sublead_photon_type = 2;   // fake from  electron 
                    }
                    else
                        sublead_photon_type = 1; // prompt

                    // Do our own gen-matching
                    double lead_photon_closest_match = dipho->leadingPhoton()->genMatchType() == 1 ? sqrt( pow(lead_photon_eta - dipho->leadingPhoton()->matchedGenPhoton()->eta(), 2) + pow(lead_photon_phi - dipho->leadingPhoton()->matchedGenPhoton()->phi(), 2)) : 999;
                    double sublead_photon_closest_match = dipho->subLeadingPhoton()->genMatchType() == 1 ? sqrt( pow(sublead_photon_eta - dipho->subLeadingPhoton()->matchedGenPhoton()->eta(), 2) + pow(sublead_photon_phi - dipho->subLeadingPhoton()->matchedGenPhoton()->phi(), 2)) : 999;

                    double lead_photon_closest_match_pt = dipho->leadingPhoton()->genMatchType() == 1 ? dipho->leadingPhoton()->pt() : -1;
                    double sublead_photon_closest_match_pt = dipho->subLeadingPhoton()->genMatchType() == 1 ? dipho->subLeadingPhoton()->pt() : -1;
                    for( unsigned int genLoop = 0 ; genLoop < genParticles->size(); genLoop++ ) {
                        int pdgid = genParticles->ptrAt( genLoop )->pdgId();
                        if (abs(pdgid) != 22) continue;
                        if (genParticles->ptrAt( genLoop )->p4().pt() < 10) continue;
                        if (!genParticles->ptrAt( genLoop )->isPromptFinalState()) continue;
                        double gen_photon_candidate_eta = genParticles->ptrAt( genLoop )->p4().eta();
                        double gen_photon_candidate_phi = genParticles->ptrAt( genLoop )->p4().phi();

                        double deltaR_lead = sqrt( pow(gen_photon_candidate_eta - lead_photon_eta, 2) + pow(gen_photon_candidate_phi - lead_photon_phi, 2));
                        double deltaR_sublead = sqrt( pow(gen_photon_candidate_eta - sublead_photon_eta, 2) + pow(gen_photon_candidate_phi - sublead_photon_phi, 2));

                        if (deltaR_lead < lead_photon_closest_match) {   
                            lead_photon_closest_match = deltaR_lead;
                            lead_photon_closest_match_pt = genParticles->ptrAt( genLoop )->p4().pt();
                        }
                        if (deltaR_sublead < sublead_photon_closest_match) {
                            sublead_photon_closest_match = deltaR_sublead;
                            sublead_photon_closest_match_pt = genParticles->ptrAt( genLoop )->p4().pt();
                        }
                    } // end gen loop
                    string lead_fake = dipho->leadingPhoton()->genMatchType() == 1 ? "prompt" : "fake";
                    //cout << "Leading photon is " << lead_fake << ". Closest match:" << lead_photon_closest_match << endl;

                    string sublead_fake = dipho->subLeadingPhoton()->genMatchType() == 1 ? "prompt" : "fake";
                    //cout << "Subleading photon is " << sublead_fake << ". Closest match:" << sublead_photon_closest_match << endl;


                    tthhtags_obj.setnGoodEls(nGoodEls);
                    tthhtags_obj.setnGoodElsFromTau(nGoodElsFromTau);
                    tthhtags_obj.setnGoodMus(nGoodMus);
                    tthhtags_obj.setnGoodMusFromTau(nGoodMusFromTau);
                    tthhtags_obj.setnGoodTaus(nGoodTaus);
                    tthhtags_obj.setLeadPhotonType(lead_photon_type);
                    tthhtags_obj.setSubleadPhotonType(sublead_photon_type); 
                    tthhtags_obj.setLeadPhotonClosestDeltaR(lead_photon_closest_match);
                    tthhtags_obj.setSubleadPhotonClosestDeltaR(sublead_photon_closest_match);
                    tthhtags_obj.setLeadPhotonClosestPt(lead_photon_closest_match_pt);
                    tthhtags_obj.setSubleadPhotonClosestPt(sublead_photon_closest_match_pt);
                }
                else {
                    tthhtags_obj.setnGoodEls(-1);
                    tthhtags_obj.setnGoodElsFromTau(-1);
                    tthhtags_obj.setnGoodMus(-1);
                    tthhtags_obj.setnGoodMusFromTau(-1);
                    tthhtags_obj.setnGoodTaus(-1);
                    tthhtags_obj.setLeadPhotonType(-1);
                    tthhtags_obj.setSubleadPhotonType(-1);
                    tthhtags_obj.setLeadPhotonClosestDeltaR(-1);
                    tthhtags_obj.setSubleadPhotonClosestDeltaR(-1);
                    tthhtags_obj.setLeadPhotonClosestPt(-1);
                    tthhtags_obj.setSubleadPhotonClosestPt(-1);
                }	


                if(!useTTHHadronicMVA_){
                    for( unsigned num = 0; num < JetVect.size(); num++ ) {
                        tthhtags_obj.includeWeightsByLabel( *JetVect[num] , "JetBTagCutWeight");
                        //tthhtags_obj.includeWeightsByLabel( *JetVect[num] , "JetBTagReshapeWeight");
                    }
                } else {
                    for( unsigned num = 0; num < JetVect.size(); num++ ) {
                        tthhtags_obj.includeWeightsByLabel( *JetVect[num] , "JetBTagReshapeWeight");
                    }                    
                }
                tthhtags_obj.includeWeights( *dipho );

                tthhtags->push_back( tthhtags_obj );
                if( ! evt.isRealData() ) {
                    TagTruthBase truth_obj;
                    truth_obj.setGenPV( higgsVtx );
                    if ( stage0cat.isValid() ) {
                        truth_obj.setHTXSInfo( *( stage0cat.product() ),
                                               *( stage1cat.product() ),
                                               *( njets.product() ),
                                               *( pTH.product() ),
                                               *( pTV.product() ) );
                    } else if ( htxsClassification.isValid() ) {
                        truth_obj.setHTXSInfo( htxsClassification->stage0_cat,
                                               htxsClassification->stage1_cat_pTjet30GeV,
                                               htxsClassification->jets30.size(),
                                               htxsClassification->p4decay_higgs.pt(),
                                               htxsClassification->p4decay_V.pt() );

                    } else {
                        truth_obj.setHTXSInfo( 0, 0, 0, 0., 0. );
                    }
                    //truth_obj.setGenJets(genJetVect);
                    truths->push_back( truth_obj );
                    tthhtags->back().setTagTruth( edm::refToPtr( edm::Ref<vector<TagTruthBase> >( rTagTruth, idx++ ) ) );
                }
                // count++;
            }
        }
        evt.put( std::move( tthhtags ) );
        evt.put( std::move( truths ) );
        // cout << "tagged events = " << count << endl;
    }
}
typedef flashgg::TTHHadronicTagProducer FlashggTTHHadronicTagProducer;
DEFINE_FWK_MODULE( FlashggTTHHadronicTagProducer );


