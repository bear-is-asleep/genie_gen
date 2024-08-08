#!/usr/bin/env python3

import argparse
import subprocess
import os
import concurrent.futures

CPUS = int(os.cpu_count()/2) # Number of CPUs to use
CPUS = 10

def source_constants(file_path):
    """Source the constants from the given file."""
    with open(file_path) as f:
        code = compile(f.read(), file_path, 'exec')
        exec(code, globals())

def determine_neutrino(nupdg):
    """Determine the neutrino type based on NUPDG."""
    neutrino_map = {
        "14": "numu",
        "-14": "numubar",
        "12": "nue",
        "-12": "nuebar"
    }
    return neutrino_map.get(nupdg, "Unknown")

def do_work(j):
    """Function to be run in parallel."""
    #print(FLUX_STR)
    #print(NUPDG, TGT, NEVTS, GEN, TUNE, MAXE, MINE, SPLINES, SEED, j, FLUX_STR, i)
    seed = str(SEED + j)
    cmd = [
        'gevgen',
        '-p', NUPDG,
        '-t', TGT,
        '--event-generator-list', GEN,
        '--tune', TUNE,
        '-n', str(int(round(NEVTS/CPUS))),
        '-e', f"{MINE},{MAXE}",
        '-f', f"{FLUX_STR}", #TODO: add handling for different prism fluxes
        '--cross-sections', SPLINES,
        '--seed', seed
    ]
    #Make dir to do work in
    workdir = os.path.join(optarg, f'seed{seed}')
    os.makedirs(workdir, exist_ok=True)
    print('Submitted: ', ' '.join(cmd),'\n')
    try:
        result = subprocess.run(cmd, check=True, capture_output=True, text=True, cwd=workdir)
        print('Command output:', result.stdout)
        print('Command error (if any):', result.stderr)
        
        # source = os.path.join(workdir, 'gntp.0.ghep.root')
        # destination = os.path.join(optarg, f'gntp.prism_bin{seed}.ghep.root')
        # if os.path.exists(source):
        #     os.rename(source, destination)
        #     print(f'Renamed {source} to {destination}')
        # else:
        #     print(f'Source file {source} does not exist')
    except subprocess.CalledProcessError as e:
        print(f'Command {" ".join(cmd)}failed with error: {e.returncode}')
        print(f'Stdout: {e.stdout}')
        print(f'Stderr: {e.stderr}')
    except Exception as e:
        print('An error occurred:', e)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Make splines for GENIE")
    parser.add_argument('-c', required=True, metavar='CONFIG_DIR', help="Directory containing the configuration file (configure.py) and will also be where the data goes.")
    args = parser.parse_args()
    
    # Source the constants file
    optarg = args.c
    source_constants(os.path.join(optarg, 'configure.py'))
    
    # Determine the neutrino type
    NEUTRINO = determine_neutrino(NUPDG)
    
    # Run the work in parallel
    with concurrent.futures.ThreadPoolExecutor() as executor:
        print(f"Running {CPUS} jobs in parallel.")
        executor.map(do_work, range(CPUS))
