#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "beargit.h"
#include "util.h"

/* Implementation Notes:
 *
 * - Functions return 0 if successful, 1 if there is an error.
 * - All error conditions in the function description need to be implemented
 *   and written to stderr. We catch some additional errors for you in main.c.
 * - Output to stdout needs to be exactly as specified in the function description.
 * - Only edit this file (beargit.c)
 * - You are given the following helper functions:
 *   * fs_mkdir(dirname): create directory <dirname>
 *   * fs_rm(filename): delete file <filename>
 *   * fs_mv(src,dst): move file <src> to <dst>, overwriting <dst> if it exists
 *   * fs_cp(src,dst): copy file <src> to <dst>, overwriting <dst> if it exists
 *   * write_string_to_file(filename,str): write <str> to filename (overwriting contents)
 *   * read_string_from_file(filename,str,size): read a string of at most <size> (incl.
 *     NULL character) from file <filename> and store it into <str>. Note that <str>
 *     needs to be large enough to hold that string.
 *  - You NEED to test your code. The autograder we provide does not contain the
 *    full set of tests that we will run on your code. See "Step 5" in the homework spec.
 */

/* beargit init
 *
 * - Create .beargit directory
 * - Create empty .beargit/.index file
 * - Create .beargit/.prev file containing 0..0 commit id
 *
 * Output (to stdout):
 * - None if successful
 */

const char* zero_commit = "0000000000000000000000000000000000000000";

int beargit_init(void) {
	fs_mkdir(".beargit");

	FILE* findex = fopen(".beargit/.index", "w");
	fclose(findex);

	FILE* fbranches = fopen(".beargit/.branches", "w");
	fprintf(fbranches, "%s\n", "master");
	fclose(fbranches);

	write_string_to_file(".beargit/.prev", zero_commit);
	write_string_to_file(".beargit/.current_branch", "master");

	return 0;
}


/* beargit add <filename>
 * 
 * - Append filename to list in .beargit/.index if it isn't in there yet
 *
 * Possible errors (to stderr):
 * >> ERROR: File <filename> already added
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_add(const char* filename) {
	FILE* findex = fopen(".beargit/.index", "r");
	FILE *fnewindex = fopen(".beargit/.newindex", "w");

	char line[FILENAME_SIZE];
	while(fgets(line, sizeof(line), findex)) {
		strtok(line, "\n");
		if (strcmp(line, filename) == 0) {
			fprintf(stderr, "ERROR: File %s already added\n", filename);
			fclose(findex);
			fclose(fnewindex);
			fs_rm(".beargit/.newindex");
			return 3;
		}

		fprintf(fnewindex, "%s\n", line);
	}

	fprintf(fnewindex, "%s\n", filename);
	fclose(findex);
	fclose(fnewindex);

	fs_mv(".beargit/.newindex", ".beargit/.index");

	return 0;
}


/* beargit rm <filename>
 * 
 * See "Step 2" in the homework 1 spec.
 *
 */

int beargit_rm(const char* filename) {
	/* COMPLETE THE REST */
	FILE* findex = fopen(".beargit/.index","r");
	FILE* fnewindex = fopen(".beargit/.newindex","w");

	char line[FILENAME_SIZE];
	int found;
	found = 0;
	while(fgets(line, sizeof(line), findex))
	{
		strtok(line,"\n");
		if(strcmp(line, filename) == 0)
		{
			found = 1; 
			continue;
		}
		fprintf(fnewindex, "%s\n", line);
	}
	fclose(findex);
	fclose(fnewindex);
	if(found == 0)
	{
		fprintf(stderr, "Error: File %s not tracked\n", filename);
		return 1;
	}
	else
	{
		fs_mv(".beargit/.newindex", ".beargit/.index");
		return 0;
	}
}

/* beargit commit -m <msg>
 *
 * See "Step 3" in the homework 1 spec.
 *
 */

const char* go_bears = "GO BEARS!";
const int go_bears_length = 9;
int is_commit_msg_ok(const char* msg) {
	/* COMPLETE THE REST */
	int length;
	length = strlen(msg);
	int index;
	int compareIndex;
	compareIndex = 0;
	for (index = 0; index < length; ++index)
	{
		if( msg[index] == go_bears[compareIndex])
		{
			++compareIndex;
			if(compareIndex == go_bears_length) {
				return 1;
			}
		}
		else
		{
			if(compareIndex != 0)
			{
				compareIndex = 0;
				--index;
			}
		}

	}
	return 0;
}

void cycleThroughLetter(char* commit_id, int letterIndex)
{
	char* currentLetterPos = commit_id + letterIndex;
	char currentLetter = *currentLetterPos;
    if(currentLetter == '0')
    {
		*currentLetterPos = '6';
		letterIndex = letterIndex + 1;
		letterIndex = letterIndex % commit_id_bytes;
		cycleThroughLetter(commit_id, letterIndex);
    }
	if(currentLetter == '6')
	{
		*currentLetterPos = '1';
	}
	else if(currentLetter == '1')
	{
		*currentLetterPos = 'C';
	}
	else if (currentLetter == 'C')
	{
		*currentLetterPos = '6';
		letterIndex = letterIndex + 1;
		letterIndex = letterIndex % commit_id_bytes;
		cycleThroughLetter(commit_id, letterIndex);
	}
}

void next_commit_id_hw1(char* commit_id) {
	/* COMPLETE THE REST */
	cycleThroughLetter(commit_id, 0);
}
const char* beargit_dir = ".beargit/";

void copy_file_to_directory(const char* filename, const char* directory)
{
	char newFilePath[commit_path_length + FILENAME_SIZE];
	sprintf(newFilePath, "%s/%s", directory, filename);
	fs_cp(filename, newFilePath);
}

int beargit_commit_hw1(const char* msg) {
	if (!is_commit_msg_ok(msg)) {
		fprintf(stderr, "ERROR: Message must contain \"%s\"\n", go_bears);
		return 1;
	}

	char commit_id[COMMIT_ID_SIZE];
	read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
	next_commit_id(commit_id);
	/* COMPLETE THE REST */
    char commit_path[commit_path_length] = ".beargit/"; 
    sprintf(commit_path, "%s%s/", beargit_dir, commit_id);
    fs_mkdir(commit_path);
    char dirString [commit_beargit_length];
    sprintf(dirString, "%s.beargit", commit_path);
    fs_mkdir(dirString);
    char file[FILENAME_SIZE];
    FILE* findex = fopen(".beargit/.index", "r");
    while(fgets(file, sizeof(file), findex))
    {
      strtok(file, "\n");
      copy_file_to_directory(file, commit_path);
    } 
    fclose(findex);
    copy_file_to_directory(".beargit/.index", commit_path);
    copy_file_to_directory(".beargit/.prev", commit_path);
    write_string_to_file(".msg", msg);
    copy_file_to_directory(".msg", commit_path);
    write_string_to_file(".beargit/.prev", commit_id);
    return 0;
}

/* beargit status
 *
 * See "Step 1" in the homework 1 spec.
 *
 */

int beargit_status() {
	/* COMPLETE THE REST */
	char line[FILENAME_SIZE];
	FILE* findex = fopen(".beargit/.index", "r");
	printf("Tracked files:\n");
	while(fgets(line, sizeof(line), findex))
	{
		fprintf(stdout, "%s", line);
	}
	fclose(findex);
	return 0;
}

/* beargit log
 *
 * See "Step 4" in the homework 1 spec.
 */

void print_log(const char* commit_id, const char* message)
{
	fprintf(stdout, "\n commit %s\n\t%s", commit_id, message);
}

int beargit_log() {
	/* COMPLETE THE REST*/
	char prev_commit_id[COMMIT_ID_SIZE];
	read_string_from_file(".beargit/.prev", prev_commit_id, COMMIT_ID_SIZE);
	char prev_commit_path[commit_beargit_length + 10];
	char prev_commit_msg[commit_beargit_length];
	char msg[MSG_SIZE];
	while(1)
	{
		sprintf(prev_commit_msg, ".beargit/%s/.msg", prev_commit_id);
		FILE* msgfile = fopen(prev_commit_msg, "r");
		if(msgfile == NULL){
			break;
		}
		fgets(msg, MSG_SIZE, msgfile);
		print_log(prev_commit_id, msg);
		sprintf(prev_commit_path, ".beargit/%s/.beargit/.prev", prev_commit_id);
		read_string_from_file(prev_commit_path, prev_commit_id, COMMIT_ID_SIZE);
	}
	printf("\n");
	return 0;
}

// ---------------------------------------
// HOMEWORK 2 
// ---------------------------------------

// This adds a check to beargit_commit that ensures we are on the HEAD of a branch.
int beargit_commit(const char* msg) {
	char current_branch[BRANCHNAME_SIZE];
	read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);

	if (strlen(current_branch) == 0) {
		fprintf(stderr, "ERROR: Need to be on HEAD of a branch to commit\n");
		return 1;
	}

	return beargit_commit_hw1(msg);
}

const char* digits = "61c";

void next_commit_id(char* commit_id) {
	char current_branch[BRANCHNAME_SIZE];
	read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);

	// The first COMMIT_ID_BRANCH_BYTES=10 characters of the commit ID will
	// be used to encode the current branch number. This is necessary to avoid
	// duplicate IDs in different branches, as they can have the same pre-
	// decessor (so next_commit_id has to depend on something else).
	int n = get_branch_number(current_branch);
	for (int i = 0; i < COMMIT_ID_BRANCH_BYTES; i++) {
		// This translates the branch number into base 3 and substitutes 0 for
		// 6, 1 for 1 and c for 2.
		commit_id[i] = digits[n%3];
		n /= 3;
	}

	// Use next_commit_id to fill in the rest of the commit ID.
	next_commit_id_hw1(commit_id + COMMIT_ID_BRANCH_BYTES);
}


// This helper function returns the branch number for a specific branch, or
// returns -1 if the branch does not exist.
int get_branch_number(const char* branch_name) {
	FILE* fbranches = fopen(".beargit/.branches", "r");

	int branch_index = -1;
	int counter = 0;
	char line[FILENAME_SIZE];
	while(fgets(line, sizeof(line), fbranches)) {
		strtok(line, "\n");
		if (strcmp(line, branch_name) == 0) {
			branch_index = counter;
		}
		counter++;
	}

	fclose(fbranches);

	return branch_index;
}

/* beargit branch
 *
 * See "Step 6" in the homework 1 spec.
 *
 */

void load_current(char* current_branch, int* on_branch)
{
	FILE* current = fopen(".beargit/.current_branch", "r");
	int i = 0;
	char c;

	do
	{
		c = fgetc(current);
		current_branch[i++] = c;
	}
	while(c != EOF);

	if(i > 0)
	{
		*on_branch = 1;
	}
	else
	{
		*on_branch = 0;
	}
}

const char* star = "*";

int beargit_branch() {
	/* COMPLETE THE REST */
	FILE* fbranches = fopen(".beargit/.branches","r");
	char current_branch[BRANCHNAME_SIZE];
	int on_branch;
	load_current(current_branch, &on_branch);
	char line[BRANCHNAME_SIZE];
	int line_count = 0;
    fpos_t pos;
    fgetpos(fbranches, &pos);
	while(fgets(line, sizeof(line), fbranches))
	{
      ++line_count;
	}
	while(line_count > 0)
	{
      fsetpos(fbranches, &pos);
      fseek(fbranches, BRANCHNAME_SIZE * (line_count -1), SEEK_SET);
      fgets(line, sizeof(line), fbranches);
      strtok(line, "\n");
      if(on_branch)
      {
        if(strcmp(line, current_branch) == 0)
        {
          strcat(line, star); 
        }
        strcat(line, "\n");
        printf("%s", line);
      }
      --line_count;
	}
	fclose(fbranches);
	return 0;
}

/* beargit checkout
 *
 * See "Step 7" in the homework 1 spec.
 *
 */

void remove_files()
{
	FILE* findex = fopen(".beargit/.index", "r");
    char file[FILENAME_SIZE];
    while (fgets(file, sizeof(file), findex))
    {
      strtok(file, "\n");
      fs_rm(file);
    }
	fclose(findex);
}

void copy_commit_index(const char* commit_id)
{
  char commit_index[commit_beargit_length + 6];
  const char* index_path = ".beargit/.index";
  sprintf(commit_index, ".beargit/%s/%s.index", commit_id, beargit_dir);
  fs_cp(commit_index, index_path);
}

void copy_files_from_index(const char* commit_id)
{
  char commit_filename[commit_path_length + FILENAME_SIZE] = ".beargit/";
  FILE* findex = fopen(".beargit/.index", "r");
  char file[FILENAME_SIZE];
  while (fgets(file, sizeof(file), findex))
  {
    strtok(file, "\n");
    sprintf(commit_filename, ".beargit/%s/%s", commit_id, file);
    fs_cp(commit_filename, file);
  }
  fclose(findex);
}

void write_commitid_to_prev(const char* commit_id)
{
	write_string_to_file(".beargit/.prev", commit_id);
}

int checkout_commit(const char* commit_id) {
	/* COMPLETE THE REST */
  remove_files();
  write_commitid_to_prev(commit_id);
  if(strcmp(commit_id, zero_commit) == 0)
  {
    write_string_to_file(".beargit/.index", "");
  }
  else
  {
    copy_commit_index(commit_id);
    copy_files_from_index(commit_id);
  }
  return 0;
}

int is_it_a_commit_id(const char* commit_id) {
	int i;
	char current_char;
	for(i = 0; i < COMMIT_ID_BYTES; ++i)
	{
		current_char = commit_id[i];
		if(current_char == '\0')
		{
			return 0;
		}
		else
		{
			if(current_char == '6' || current_char == '1' || current_char == 'C')
			{
				continue;
			}
			else
			{
				return 0;
			}
		}
	}
	return 1;
}

int beargit_checkout(const char* arg, int new_branch) {
	// Get the current branch
	char current_branch[BRANCHNAME_SIZE + 1];
    FILE* fcbranch = fopen(".beargit/.current_branch", "r");
    if(fcbranch == NULL)
    {
      perror("NO BRANCH");
    }
    fgets(current_branch, sizeof(current_branch), fcbranch);
    fclose (fcbranch);
	// If not detached, update the current branch by storing the current HEAD into that branch's file...
	// Even if we cancel later, this is still ok.
	if (strlen(current_branch))
	{
		char current_branch_file[BRANCHNAME_SIZE+50];
		sprintf(current_branch_file, ".beargit/.branch_%s", current_branch);
		fs_cp(".beargit/.prev", current_branch_file);
	}

	// Check whether the argument is a commit ID. If yes, we just stay in detached mode
	// without actually having to change into any other branch.
	if (is_it_a_commit_id(arg)) {
		char commit_dir[FILENAME_SIZE] = ".beargit/";
		strcat(commit_dir, arg);
		if (!fs_check_dir_exists(commit_dir))
		{
			fprintf(stderr, "ERROR: Commit %s does not exist\n", arg);
			return 1;
		}

		// Set the current branch to none (i.e., detached).
		write_string_to_file(".beargit/.current_branch", "");

		return checkout_commit(arg);
	}

	// Just a better name, since we now know the argument is a branch name.
	const char* branch_name = arg;

	// Read branches file (giving us the HEAD commit id for that branch).
	int branch_exists = (get_branch_number(branch_name) >= 0);

	// Check for errors.
	// error1
	if (branch_exists && new_branch)
	{
		fprintf(stderr, "ERROR: A branch named %s already exists\n", branch_name);
		return 1;
	} 
	//error2
	else if (!branch_exists && !new_branch)
	{
		fprintf(stderr, "ERROR: No branch %s exists\n", branch_name);
		return 1;
	}

	// File for the branch we are changing into.
	char branch_file[BRANCHNAME_SIZE + 18] = ".beargit/.branch_"; 
	strcat(branch_file, branch_name);

	// Update the branch file if new branch is created (now it can't go wrong anymore)
	if (new_branch)
	{
		FILE* fbranches = fopen(".beargit/.branches", "a");
		fprintf(fbranches, "%s\n", branch_name);
		fclose(fbranches);
		fs_cp(".beargit/.prev", branch_file); 
	}

	write_string_to_file(".beargit/.current_branch", branch_name);

	// Read the head commit ID of this branch.
	char branch_head_commit_id[COMMIT_ID_SIZE];
	read_string_from_file(branch_file, branch_head_commit_id, COMMIT_ID_SIZE);

	// Check out the actual commit.
	return checkout_commit(branch_head_commit_id);
}
