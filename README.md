# Rootkits

## Motivation

To understand rootkits and malware, in general, you must think like the attacker, to be on the same pace, so in this project I try to encapsulate that feeling, so in this scenario you are the bad guy.

## Why would anyone do this?

This project is not about hurting other people (or other computers lol).


The development environment is Visual Studion 2015 and Windows DDK on Windows 7 x64.

The victim for the experiments is an Windows 7 32 bit VM, *why?* you may ask, because Microsoft added Kernel-Mode Code Signing using certificates for the 64 bits versions of Windows, so you are not going to add a driver anytime soon on a 64 bit system without a Microsoft approved certificate, jk..unless?

## Where to learn

Everything is based on the book **The Rootkit Arsenal Escape and Evasion in the Dark Corners of the System**, which I cannot recommend more.
