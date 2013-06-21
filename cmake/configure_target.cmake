# sets the postfixes for the various targets
function(configure_target target)
    set_target_properties(${target} PROPERTIES DEBUG_POSTFIX "-d")
    set_target_properties(${target} PROPERTIES RELWITHDEBINFO_POSTFIX "-rd")
    set_target_properties(${target} PROPERTIES MINSIZEREL_POSTFIX "-m")
endfunction()