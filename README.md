## Running

0. Setup environment `source setup.sh`
1. Make `configure.py` using template `congigure_template.py`
2. Run genie using config `python run_gevgen.py -c /path/to/configure.py`
3. Make TTrees
```shell
genie -L make_trees.C
genie > make_trees_folder("gntp.0.ghep.root", "/path/to/folder")
```

### To combine
- Combine output ttrees `hadd -f /path/to/output.root /path/to/output_*.root`
- Combine flux used `hadd -f input-flux.root seed42*/input-flux.root`