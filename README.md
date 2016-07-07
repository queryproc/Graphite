# Graphite
This project aims to build an empirical study between the [ATAC](groups.csail.mit.edu/carbon/?page_id=62) Architecture developed by the [Carbon Research Group at MIT](http://groups.csail.mit.edu/carbon/) & the [ORNOC](https://hal.archives-ouvertes.fr/file/index/docid/618600/filename/date-2011.pdf) Architecture developed by researchers from [École Central de Lyon](http://www.ec-lyon.fr/en), [École Polytechnique de Montréal](http://www.polymtl.ca/), [Concordia University](https://www.concordia.ca/).
In order to get data from the architecture, [Graphite](http://groups.csail.mit.edu/carbon/?page_id=111) an open-source simulator built by Carbon MIT is being used.

The general definition of the ORNOC architecture is taken from the paper, changed to fit the Graphite's nomenclature and implements it as a network model in Graphite.
Currently, a single reader single writer waveguide as an optical link for the ORNOC network model is in development.
