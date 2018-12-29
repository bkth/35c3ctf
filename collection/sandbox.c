#include "sandbox.h"
int bpf_resolve_jumps(struct bpf_labels *labels,
		      struct sock_filter *filter, size_t count)
{
	size_t i;

	if (count < 1 || count > BPF_MAXINSNS)
		return -1;
	/*
	* Walk it once, backwards, to build the label table and do fixups.
	* Since backward jumps are disallowed by BPF, this is easy.
	*/
	for (i = 0; i < count; ++i) {
		size_t offset = count - i - 1;
		struct sock_filter *instr = &filter[offset];
		if (instr->code != (BPF_JMP+BPF_JA))
			continue;
		switch ((instr->jt<<8)|instr->jf) {
		case (JUMP_JT<<8)|JUMP_JF:
			if (labels->labels[instr->k].location == 0xffffffff) {
				fprintf(stderr, "Unresolved label: '%s'\n",
					labels->labels[instr->k].label);
				return 1;
			}
			instr->k = labels->labels[instr->k].location -
				    (offset + 1);
			instr->jt = 0;
			instr->jf = 0;
			continue;
		case (LABEL_JT<<8)|LABEL_JF:
			if (labels->labels[instr->k].location != 0xffffffff) {
				fprintf(stderr, "Duplicate label use: '%s'\n",
					labels->labels[instr->k].label);
				return 1;
			}
			labels->labels[instr->k].location = offset;
			instr->k = 0; /* fall through */
			instr->jt = 0;
			instr->jf = 0;
			continue;
		}
	}
	return 0;
}

/* Simple lookup table for labels. */
__u32 seccomp_bpf_label(struct bpf_labels *labels, const char *label)
{
	struct __bpf_label *begin = labels->labels, *end;
	int id;

	if (labels->count == BPF_LABELS_MAX) {
		fprintf(stderr, "Too many labels\n");
		exit(1);
	}
	if (labels->count == 0) {
		begin->label = label;
		begin->location = 0xffffffff;
		labels->count++;
		return 0;
	}
	end = begin + labels->count;
	for (id = 0; begin < end; ++begin, ++id) {
		if (!strcmp(label, begin->label))
			return id;
	}
	begin->label = label;
	begin->location = 0xffffffff;
	labels->count++;
	return id;
}

void seccomp_bpf_print(struct sock_filter *filter, size_t count)
{
	struct sock_filter *end = filter + count;
	for ( ; filter < end; ++filter)
		printf("{ code=%u,jt=%u,jf=%u,k=%u },\n",
			filter->code, filter->jt, filter->jf, filter->k);
}
void
init_sandbox() {
	struct bpf_labels l = {
		.count = 0,
	};
	struct sock_filter filter[] = {
	VALIDATE_ARCHITECTURE,
	LOAD_SYSCALL_NR,
	SECCOMP_SYSCALL(__NR_exit, ALLOW),
	SECCOMP_SYSCALL(__NR_exit_group, ALLOW),
	SECCOMP_SYSCALL(__NR_brk, ALLOW),
	SECCOMP_SYSCALL(__NR_mmap, JUMP(&l, mmap)),
	SECCOMP_SYSCALL(__NR_munmap, ALLOW),
	SECCOMP_SYSCALL(__NR_mremap, ALLOW),
	SECCOMP_SYSCALL(__NR_readv, ALLOW),
	SECCOMP_SYSCALL(__NR_futex, ALLOW),
	SECCOMP_SYSCALL(__NR_sigaltstack, ALLOW),
	SECCOMP_SYSCALL(__NR_close, ALLOW),
	SECCOMP_SYSCALL(__NR_write, JUMP(&l, write)),
	SECCOMP_SYSCALL(__NR_rt_sigaction, ALLOW),
	DENY,

	LABEL(&l, mmap),
	ARG(0),
	JNE(0, DENY),
	ARG(2),
	JNE(PROT_READ|PROT_WRITE, DENY),
	ARG(3),
	JNE(MAP_PRIVATE|MAP_ANONYMOUS, DENY),
    ARG(4),
    JNE(0xffffffff, DENY),
    ARG(5),
    JNE(0, DENY),
	ALLOW,

	LABEL(&l, write),
	ARG(0),
	JEQ(STDOUT_FILENO, ALLOW),
	JEQ(STDERR_FILENO, ALLOW),
	DENY,
	};
	struct sock_fprog prog = {
		.filter = filter,
		.len = (unsigned short)(sizeof(filter)/sizeof(filter[0])),
	};
	bpf_resolve_jumps(&l, filter, sizeof(filter)/sizeof(*filter));

	if (syscall(__NR_prctl, PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0)) {
		err(1, "prctl(NO_NEW_PRIVS)");
	}

	if (prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &prog)) {
		err(1, "prctl(SECCOMP)");
	}
}
