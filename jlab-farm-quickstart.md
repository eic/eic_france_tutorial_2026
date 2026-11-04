---
title: "JLab Farm Quick Start"
---

If you will be running eic-shell on the JLab farm systems, some pointers for getting started on the
JLab farm are included below.

## Connecting to an iFarm node

Before connecting to an iFarm node, you must first connect to the login node via -

```bash
ssh USER@login.jlab.org
```

Where you should replace USER with your username. You will be prompted to enter your password. For
the login node this is your PIN + two-factor token. Do not insert a space between the PIN and token.
Once on the login node, you can then connect to an interactive node via -

```bash
ssh USER@ifarm9
```

You will again be prompted for your password, this time, it's your CUE password.

Other iFarm nodes can be specified if desired. Additionally, you can also just use -

```bash
ssh -J USER@login.jlab.org USER@ifarm9
```

to do both steps at once.

See this [Service Now](https://jlab.servicenowservices.com/scicomp?id=kb_article_view&sysparm_article=KB0015066)
documentation for further pointers and assistance.

For running larger/longer simulation/reconstruction jobs, you may find the
[scripts](https://github.com/eic/snippets/tree/main/JLab_Farm_Scripts) in the snippets repository
useful.
