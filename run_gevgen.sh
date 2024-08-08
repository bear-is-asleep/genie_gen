#!/bin/bash
# Purpose: Make splines for GENIE

# A while loop to read the config
while getopts ":c:" opt; do
  case ${opt} in
    c)
      # Source the constants file
      source ${OPTARG}/configure.sh
      ;;
    \?)
      echo "Invalid option: $OPTARG" 1>&2
      ;;
    :)
      echo "Invalid option: $OPTARG requires an argument" 1>&2
      ;;
  esac
done

#Make if statement to set name of neutrino given NUPDG
if [ ${NUPDG} == "14" ]; then
  NEUTRINO="numu"
elif [ ${NUPDG} == "-14" ]; then
  NEUTRINO="numubar"
elif [ ${NUPDG} == "12" ]; then
  NEUTRINO="nue"
elif [ ${NUPDG} == "-12" ]; then
  NEUTRINO="nuebar"
fi

#Boolean to use PRISM flux or nominal
if [ ${FLUX} == "PRISM" ]; then
  FLUX_STR="/exp/sbnd/data/users/brindenc/genie/crpa/gevgens/sbnd_flux.root,flux_sbnd_${NEUTRINO}"
else
  FLUX_STR="/sbnd/data/users/brindenc/genie/crpa/gevgens/sbnd_flux.root,flux_sbnd_${NEUTRINO}"
fi


# Define a function to be run in parallel
do_work() {
  j=$1
  echo gevgen \
    -p ${NUPDG} -t ${TGT} --event-generator-list ${GEN} --tune ${TUNE} -n ${NEVTS} -e ${MINE},${MAXE} \
    -f ${FLUX_STR}${i} --cross-sections ${SPLINES} --seed ${j+10}

  gevgen \
    -p ${NUPDG} -t ${TGT} --event-generator-list ${GEN} --tune ${TUNE} -n ${NEVTS} -e ${MINE},${MAXE} \
    -f ${FLUX_STR}${i} --cross-sections ${SPLINES} --seed ${j+10}
  
  mv ${OPTARG}/gntp.0.ghep.root ${OPTARG}/gntp.prism_bin${j}.ghep.root
}

# Export the function so it's available to parallel
export -f do_work

# Use parallel to run the function in parallel
parallel do_work ::: {0..10}