package awais.instagrabber.adapters.viewholder.directmessages;

import android.text.Spannable;
import android.text.SpannableStringBuilder;
import android.text.style.ClickableSpan;
import android.text.style.ForegroundColorSpan;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.ItemTouchHelper;

import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.adapters.DirectItemsAdapter.DirectItemCallback;
import awais.instagrabber.databinding.LayoutDmActionLogBinding;
import awais.instagrabber.databinding.LayoutDmBaseBinding;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectItemActionLog;
import awais.instagrabber.repositories.responses.directmessages.DirectItemVideoCallEvent;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.utils.TextUtils;

public class DirectItemVideoCallEventViewHolder extends DirectItemViewHolder {

    private final LayoutDmActionLogBinding binding;

    public DirectItemVideoCallEventViewHolder(@NonNull final LayoutDmBaseBinding baseBinding,
                                              final LayoutDmActionLogBinding binding,
                                              final User currentUser,
                                              final DirectThread thread,
                                              final DirectItemCallback callback) {
        super(baseBinding, currentUser, thread, callback);
        this.binding = binding;
        setItemView(binding.getRoot());
    }

    @Override
    public void bindItem(final DirectItem directItemModel, final MessageDirection messageDirection) {
        final DirectItemVideoCallEvent videoCallEvent = directItemModel.getVideoCallEvent();
        final String text = videoCallEvent.getDescription();
        final SpannableStringBuilder sb = new SpannableStringBuilder(text);
        final List<DirectItemActionLog.TextRange> textAttributes = videoCallEvent.getTextAttributes();
        if (textAttributes != null && !textAttributes.isEmpty()) {
            for (final DirectItemActionLog.TextRange textAttribute : textAttributes) {
                if (!TextUtils.isEmpty(textAttribute.getColor())) {
                    final ForegroundColorSpan colorSpan = new ForegroundColorSpan(itemView.getResources().getColor(R.color.deep_orange_400));
                    sb.setSpan(colorSpan, textAttribute.getStart(), textAttribute.getEnd(), Spannable.SPAN_INCLUSIVE_INCLUSIVE);
                }
                if (!TextUtils.isEmpty(textAttribute.getIntent())) {
                    final ClickableSpan clickableSpan = new ClickableSpan() {
                        @Override
                        public void onClick(@NonNull final View widget) {

                        }
                    };
                    sb.setSpan(clickableSpan, textAttribute.getStart(), textAttribute.getEnd(), Spannable.SPAN_INCLUSIVE_INCLUSIVE);
                }
            }
        }
        binding.tvMessage.setMaxLines(1);
        binding.tvMessage.setText(sb);
    }

    @Override
    protected boolean allowMessageDirectionGravity() {
        return false;
    }

    @Override
    protected boolean showUserDetailsInGroup() {
        return false;
    }

    @Override
    protected boolean showMessageInfo() {
        return false;
    }

    @Override
    protected boolean allowLongClick() {
        return false;
    }

    @Override
    public int getSwipeDirection() {
        return ItemTouchHelper.ACTION_STATE_IDLE;
    }
}
