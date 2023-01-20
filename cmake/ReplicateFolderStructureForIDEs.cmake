# This function replicates the directory hierarchy of a target, so that the
# layout of the files in IDEs match
function(replicate_directory_structure target_name)
  # Add any files that are not in the source list to a separate group
  source_group("other files" REGULAR_EXPRESSION ".")
  get_target_property(target_sources "${target_name}" SOURCES)

  foreach(file_name IN LISTS target_sources)
    get_filename_component(directory_name "${file_name}" PATH)
    # Group names use '\\' instead of '/'
    string(REPLACE "/" "\\" group_name "${directory_name}")
    source_group("${group_name}" FILES "${file_name}")
  endforeach()
endfunction()
