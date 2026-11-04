---
title: "BNL SDCC Quick Start"
---

If you will be running eic-shell on the BNL SDCC systems, some pointers for getting started on the
SDCC system are included below.

## Connecting to an eic interactive node

Assuming you have ssh-keys set up for the SDCC, you can connect to a login node via -

```bash
ssh -XY USER@ssh.sdcc.bnl.gov
```

Where you should replace USER with your username. This should connect you to a login node, connect
to an eic interactive node via

```bash
rterm -i
```

OR connect to a specific node via

```bash
rterm -i eicXXXX
```
