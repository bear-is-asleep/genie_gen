source /cvmfs/sbnd.opensciencegrid.org/products/sbnd/setup_sbnd.sh
setup root v6_22_08d -q e20:p392:prof
setup lhapdf v6_3_0 -q e20:p392:prof
setup log4cpp v1_1_3c -q e20:prof

export GENIE=/exp/sbnd/data/users/brindenc/genie/Generator_crpa #CRPA implementation

export PATH=$PATH:\
$ROOTSYS/bin:\
$GENIE/bin

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:\
$PYTHIA6_LIBRARY:\
$LIBXML2_LIB:\
$LHAPDF_LIB:\
$LOG4CPP_LIB:\
$ROOTSYS/lib:\
$GENIE/lib

#setup genie v3_02_00 -q e20:prof
cd $GENIE
#gmake install