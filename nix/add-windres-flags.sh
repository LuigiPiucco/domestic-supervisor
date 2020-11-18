# N.B. It may be a surprise that the derivation-specific variables are exported,
# since this is just sourced by the wrapped binaries---the end consumers. This
# is because one wrapper binary may invoke another (e.g. cc invoking ld). In
# that case, it is cheaper/better to not repeat this step and let the forked
# wrapped binary just inherit the work of the forker's wrapper script.

var_templates_list=(
    NIX_RESFLAGS_COMPILE
    NIX_RESFLAGS_COMPILE_BEFORE
    NIX_RESFLAGS_LINK
)
var_templates_bool=(
    NIX_ENFORCE_NO_NATIVE
)

accumulateRoles

# We need to mangle names for hygiene, but also take parameters/overrides
# from the environment.
for var in "${var_templates_list[@]}"; do
    mangleVarList "$var" ${role_suffixes[@]+"${role_suffixes[@]}"}
done
for var in "${var_templates_bool[@]}"; do
    mangleVarBool "$var" ${role_suffixes[@]+"${role_suffixes[@]}"}
done

# Export and assign separately in order that a failing $(..) will fail
# the script.

if [ -e @out@/nix-support/windres-resflags ]; then
    NIX_CFLAGS_COMPILE_@suffixSalt@="$(< @out@/nix-support/windres-resflags) $NIX_CFLAGS_COMPILE_@suffixSalt@"
fi

# That way forked processes will not extend these environment variables again.
export NIX_WINDRES_WRAPPER_FLAGS_SET_@suffixSalt@=1
