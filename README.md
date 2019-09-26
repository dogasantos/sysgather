cpgather - Linux Gather Information Tool

<table>
    <tr>
        <th>Version</th>
        <td>Devel</td>
    </tr>
    <tr>
        <th>Homepage</th>
        <td>#</td>
    </tr>
    <tr>
        <th>Github</th>
        <td><a href="https://github.com/dogasantos/cpgather">https://github.com/dogasantos/cpgather</a></td>
     <tr/>
    <tr>
        <th>Documentation</th>
        <td><a href="#">https://github.com/dogasantos/cpgather/wiki</a></td>
    </tr>
    <tr>
        <th>Code Documentation</th>
        <td><a href="#">soon...</a></td>
    </tr>
    <tr>
       <th>Author</th>
       <td>Douglas Secco dos Santos</td>
    </tr>
    <tr>
        <th>Twitter</th>
        <td><a href="http://twitter.com/dogasantos">@dogasantos</a></td>
    </tr>
    <tr>
        <th>date</th>
        <td>2010</td>
    </tr>
    <tr>
        <th>License</th>
        <td>GNU GPL</a></td>
    </tr>
</table>

## Synopsis

sysgather is an open source tool designed to help pentesters on a post-intrusion information gathering process.

### Objectives

Gather information during a post-shell phase on a pentest.<br>
Keep it simple

#### cpgather output 
Current supporting Standard grepable and planned xml format, both printed on standar output.

#### Credits
base64 code:
Nibble And A Half project page https://github.com/superwills/NibbleAndAHalf

#### This tool will collect

<b>Core Itens</b><br>
        - Processor info<br>
        - Memory and load average<br>
        - Mount points with special flags (nosuid,noexec,nfs shares,credentials, others)<br>
        - Kernel version, compilation date, architecture<br>
        - Kernel Metrics (Page Size, Child Max, Open Max, Stream Max, Phys Pages, Avphys Pages and others)<br>
        - Libc Version and release<br>
        - Gcc version<br>
        - Linux distro<br>
<b>Network Itens</b><br>
        - Ipv4 address and cidr prefix on all interfaces<br>
        - Ipv6 address and cidr prefix on all interfaces<br>
        - Nameservers<br>
        - Routing table<br>
        - Router capability state<br>
        - Open TCP and UDP ports<br>
<b>Security Itens</b><br>
        - Apparmor presence and state<br>
        - Selinux presence and state<br>
        - ASCII-Armor presence<br>
        - GrSec presence <br>
        - Pax Presence<br>
        - Hids and Nids Presence <br>
        - Suid Dumpable state <br>
        - ASLR state <br>
        - Minimal address map value <br>
        - List all suid files <br>
        - List all writable files <br>
        - List all writable directories <br>
        - List interesting files (htaccess, "password" named files, "users" named files, and others) <br>
        - Ssh Keys <br>
<b>Services Itens</b> <br>
        - Ssh configuration <br>
        - Snmp configuration <br>
        - Sudoers configuration  <br>
        - Available cronjobs <br>
        - List all running process<br>
<b>Mic Userspace Itens</b> <br>
        - Current user (to keep record)  <br>
        - Current bash_history interesting lines <br>
        - Current mysql_history interesting lines <br>
        - /etc/shadow interesting lines <br>

#### Future work
none, this is a dead project.

