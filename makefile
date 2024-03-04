# Включаемые директории.
# ------------------------------------------------------------------------------
incdirs := inc
libdirs :=
libs    :=

include makefile-base




# Используемые файлы.
# ------------------------------------------------------------------------------
define extras
$(patsubst res/%,%,$(wildcard res/glspec/*.rc))
endef

define source
$(patsubst src/%,%,$(wildcard src/glspec/*.cc))
endef

define object
$(addprefix ${obj}/,${extras:.rc=.o})\
$(addprefix ${obj}/,${source:.cc=.o})
endef




# Цели построения.
# ------------------------------------------------------------------------------
${obj}/%.o: src/%.cc
	$(call spot,$@)
	clang++ -o $(call path,$@) $(call path,$^) ${oflags} ${incdirs}

${obj}/%.o: res/%.rc
	$(call spot,$@)
	windres -o $(call path,$@) $(call path,$^)



${bin}/glspec.exe: $(filter ${obj}/glspec/%,${object})
	$(call spot,$@)
	clang++ -o $(call path,$@) $(call path,$^) ${libdirs} ${libs}